#pragma once
#include <thread>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <mutex>
#include <vector>
#include <iostream>
#include "joballocator.h"
#include "workstealqueue.h"
#undef min

namespace JobSystem
{
	class JobSystem
	{
	public:
		static JobSystem& Instance()
		{
			static JobSystem instance;
			return instance;
		}
		JobSystem() : m_shutdown(false){}
		~JobSystem()
		{

		}
		JobSystem(const JobSystem&) = delete;
		JobSystem& operator=(const JobSystem&) = delete;
		template<typename Function, typename... Args>
		auto Allocate(JobType type, IJob* parent, Function&& func, Args&&... args)
		{
			//according to C++ standard,multiple threads read from std::unordered_map is well defined.So there's
			//no problem here
			auto worker = m_workers[std::this_thread::get_id()];
			return worker->allocators[type].Allocate(type, parent, std::forward<Function>(func), std::forward<Args>(args)...);
		}

		//Only main thread can call run on JobSystem
		void Run()
		{
			//The calling thread is considered to be main thread.
			m_mainThreadId = std::this_thread::get_id();
			auto coreCount = std::thread::hardware_concurrency();
			if (coreCount > 0)
				coreCount--;		//exclude the calling thread
			std::vector<std::thread> threads;
			std::vector<Worker*> workers;
			for (std::size_t i = 0;i < coreCount;++i)
			{
				//Create per-thread resource for worker threads.The calling thread is also considered a worker thread
				auto worker = new Worker();
				workers.push_back(worker);
				threads.emplace_back(&Worker::Run, worker);
			}

			//Create a worker for main thread
			auto worker = new Worker();
			workers.push_back(worker);
			worker->Run();
			std::for_each(threads.begin(), threads.end(), [](auto& thread) {thread.join(); });
			std::for_each(workers.begin(), workers.end(), [](auto pWorker) {delete pWorker; });
		}

		void RunJob(IJob* job)
		{
			auto worker = m_workers[std::this_thread::get_id()];
			worker->queues[job->GetType()].Push(job);
		}

		void WaitForCompletion(IJob* job)
		{

		}

		void ShutDown()
		{
			bool expected{ false };
			if (m_shutdown.compare_exchange_strong(expected, true))
			{
				//must lock here because worker thread may modify m_workers at the same time of ShutDown call
				std::lock_guard<std::mutex> lock(m_mutex);
				std::for_each(m_workers.begin(), m_workers.end(), [](auto pair) {pair.second->running = false; });
			}
		}
	private:
		friend struct Worker;
		struct Worker
		{
			Worker()
			{
				//the allocators and queues must be construct in-place ,use some trick code to achieve it
				allocators.emplace(std::piecewise_construct, std::make_tuple(JobType::SHORT_TERM), std::make_tuple());
				allocators.emplace(std::piecewise_construct, std::make_tuple(JobType::LONG_TERM), std::make_tuple());
				queues.emplace(std::piecewise_construct, std::make_tuple(JobType::SHORT_TERM), std::make_tuple());
				queues.emplace(std::piecewise_construct, std::make_tuple(JobType::LONG_TERM), std::make_tuple());
			}
			//allocators only access through key ,so it doesn't matter if we use map or unordered map.For performance reason just use unordered map
			std::unordered_map<JobType, JobAllocator> allocators;
			//we should always schedule short-term job before long-term job,so use map to ensure order
			std::map<JobType, WorkStealQueue> queues;
			bool running{ true };

			void DoRun()
			{
				static int i = 0;
				i++;
				auto func = []() {std::cout << "Thread id is " << std::this_thread::get_id() << std::endl;};
				auto job = JobSystem::Instance().Allocate(JobType::SHORT_TERM, nullptr, func);
				job->Execute();
				std::cout << "i is " << i << std::endl;
				if (i == 20000)
				{
					JobSystem::Instance().ShutDown();
					std::cout << "Shut down!!!!" << std::endl;
				}
			}

			void Run()
			{
				auto& system = JobSystem::Instance();
				{
					std::lock_guard<std::mutex> lock(system.m_mutex);
					//always emplace no matter if the system is already shut down.Delay 
					//worker delete to main thread
					system.m_workers.emplace(std::make_pair(std::this_thread::get_id(), this));
				}
				while (running)
				{
					DoRun();
				}
				//Don't delete worker here because other threads may wait for a job allocated by this thread.Delete here will cause dangling pointer issue
			}
		};

		std::unordered_map<std::thread::id, Worker*> m_workers;
		//only meant for initialization
		std::mutex m_mutex;
		std::thread::id m_mainThreadId;
		std::atomic<bool> m_shutdown;
	};
}
