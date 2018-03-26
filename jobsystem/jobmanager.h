#pragma once
#include <thread>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <mutex>
#include <vector>
#include <iostream>
#include <random>
#include "joballocator.h"
#include "workstealqueue.h"
#undef min

namespace JobSystem
{
	class JobManager
	{
	public:
		static JobManager& Instance()
		{
			static JobManager instance;
			return instance;
		}
		JobManager() : m_shutdown(false)
		{
		}
		~JobManager()
		{

		}
		JobManager(const JobManager&) = delete;
		JobManager& operator=(const JobManager&) = delete;
		template<typename Function, typename... Args>
		auto AllocateJob(JobType type, IJob* parent, Function&& func, Args&&... args)
		{
			//according to C++ standard,multiple threads read from std::unordered_map is well defined.So there's
			//no problem here
			auto worker = m_workers[std::this_thread::get_id()];
			return worker->allocators[type].Allocate(type, parent, std::forward<Function>(func), std::forward<Args>(args)...);
		}

		//Only main thread can call run on JobSystem
		void Run(std::function<void()> initFunc)
		{
			//The calling thread is considered to be main thread.
			m_mainThreadId = std::this_thread::get_id();
			m_initFunc = initFunc;
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
			auto worker = m_workers[std::this_thread::get_id()];
			while (!job->HasCompleted())
			{
				if (worker->running)
				{
					worker->DoRun();
				}
			}
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
				for (auto it = queues.begin(); it != queues.end();++it)
				{
					auto queue = &it->second;
					auto job = GetJob(it->first, queue);
					if (job)
					{
						job->Execute();
					}
				}
			}

			IJob* GetJob(JobType type, WorkStealQueue* queue)
			{
				auto job = queue->Pop();
				if (job)
				{
					return job;
				}
				queue = JobManager::Instance().RandomQueue(type, this);
				if (queue)
				{
					return queue->Steal();
				}
				return nullptr;
			}

			void Run()
			{
				auto& system = JobManager::Instance();
				{
					std::lock_guard<std::mutex> lock(system.m_mutex);
					//always emplace no matter if the system is already shut down.Delay 
					//worker delete to main thread
					system.m_workers.emplace(std::make_pair(std::this_thread::get_id(), this));
				}
				if (std::this_thread::get_id() == JobManager::Instance().m_mainThreadId)
				{
					JobManager::Instance().m_initFunc();
				}
				while (running)
				{
					DoRun();
				}
				//Don't delete worker here because other threads may wait for a job allocated by this thread.Delete here will cause dangling pointer issue
			}
		};

		WorkStealQueue* RandomQueue(JobType type, Worker* exclude)
		{
			auto queueCount = m_workers.size() - 1;
			if (queueCount <= 0)
				return nullptr;
			std::random_device rd;
			std::mt19937 engine(rd());
			std::uniform_int_distribution<int> dist(0, queueCount - 1);
			auto value = dist(engine);
			int i{ 0 };
			for (auto it = m_workers.begin(); it != m_workers.end();++it)
			{
				if (it->second == exclude)
					continue;
				if (i == value)
					return &it->second->queues[type];
				++i;
			}
			return nullptr;
		}

		std::unordered_map<std::thread::id, Worker*> m_workers;
		//only meant for initialization
		std::mutex m_mutex;
		std::thread::id m_mainThreadId;
		std::atomic<bool> m_shutdown;
		std::function<void()> m_initFunc;
	};
}
