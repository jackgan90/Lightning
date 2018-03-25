#pragma once
#include <thread>
#include <algorithm>
#include <unordered_map>
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
		auto Allocate(IJob* parent, Function&& func, Args&&... args)
		{
			//according to C++ standard,multiple threads read from std::unordered_map is well defined.So there's
			//no problem here
			auto worker = m_workers[std::this_thread::get_id()];
			return worker->allocator.Allocate(parent, std::forward<Function>(func), std::forward<Args>(args)...);
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
			for (std::size_t i = 0;i < coreCount;++i)
			{
				//Create per-thread resource for worker threads.The calling thread is also considered a worker thread
				auto worker = new Worker();
				threads.emplace_back(&Worker::Run, worker);
			}

			//Create a worker for main thread
			auto worker = new Worker();
			worker->Run();
			for (auto& thread : threads)
			{
				if(thread.get_id() != std::this_thread::get_id())
					thread.join();
			}
			threads.clear();
		}

		void RunJob(IJob* job)
		{
			auto worker = m_workers[std::this_thread::get_id()];
			worker->queue.Push(job);
		}

		void WaitForCompletion(IJob* job)
		{

		}

		void ShutDown()
		{
			bool expected{ false };
			if (m_shutdown.compare_exchange_strong(expected, true))
				return;
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
			JobAllocator allocator;
			WorkStealQueue queue;
			bool running{ true };

			void DoRun()
			{
				auto func = []() {std::cout << "Thread id is " << std::this_thread::get_id() << std::endl;};
				auto job = allocator.Allocate(nullptr, func);
				job->Execute();
			}

			void Run()
			{
				auto& system = JobSystem::Instance();
				{
					std::lock_guard<std::mutex> lock(system.m_mutex);
					if (system.m_shutdown)
					{
						running = false;
					}
					else
					{
						system.m_workers.emplace(std::make_pair(std::this_thread::get_id(), this));
					}
				}
				while (running)
				{
					DoRun();
				}
				{
					std::lock_guard<std::mutex> lock(system.m_mutex);
					system.m_workers.erase(std::this_thread::get_id());
				}
				delete this;
			}
		};

		std::unordered_map<std::thread::id, Worker*> m_workers;
		//only meant for initialization
		std::mutex m_mutex;
		std::thread::id m_mainThreadId;
		std::atomic<bool> m_shutdown;
	};
}
