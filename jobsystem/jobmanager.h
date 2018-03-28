#pragma once
#include <thread>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <mutex>
#include <vector>
#include <iostream>
#include <random>
#include <chrono>
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
		JobManager() : m_shutdown(false), m_sleepThreadCount(0)
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
			int coreCount = std::thread::hardware_concurrency();
			m_workersCount = coreCount > 0 ? coreCount : 1;
			if (coreCount > 0)
				coreCount--;		//exclude the calling thread
			bool background{ true };
			std::vector<std::thread> threads;
			//1/4 threads serve as background worker
			for (int i = 0;i < coreCount;++i)
			{
				//Create per-thread resource for worker threads.The calling thread is also considered a worker thread
				auto worker = new Worker(background);
				if (i >= (coreCount + 1) / 4 - 1)
				{
					background = false;
				}
				m_workerVec.push_back(worker);
				threads.emplace_back(&Worker::Run, worker);
			}

			//Create a worker for main thread
			auto worker = new Worker(background);
			m_workerVec.push_back(worker);
			worker->Run();
			m_cvWake.notify_all();
			std::for_each(threads.begin(), threads.end(), [](auto& thread) {thread.join(); });
			std::for_each(m_workerVec.begin(), m_workerVec.end(), [](auto pWorker) {delete pWorker; });
			m_workerVec.clear();
		}

		void RunJob(IJob* job)
		{
			auto worker = m_workers[std::this_thread::get_id()];
			worker->queues[job->GetType()].Push(job);
			if (m_sleepThreadCount)
			{
				m_cvWake.notify_one();
			}
		}


		void WaitForCompletion(IJob* job)
		{
			auto worker = m_workers[std::this_thread::get_id()];
			while (!job->HasCompleted())
			{
				if (worker->running)
				{
					worker->DoRun(false);
				}
				else
				{
					break;
				}
			}
		}

		//get estimated background worker count.Note this method
		//may not return the exact background worker count,because we don't use lock
		//so it's just a reference value
		//This method is safe to called from any thread
		inline std::size_t GetBackgroundWorkersCount()
		{
			std::size_t workerCount{ 0 };
			std::for_each(m_workers.begin(), m_workers.end(), [&workerCount](auto pair) { workerCount += pair.second->background ? 1 : 0; });
			return workerCount;
		}

		inline std::size_t GetWorkersCount()const
		{
			return m_workersCount;
		}

		inline std::thread::id GetCurrentThreadId()const
		{
			return std::this_thread::get_id();
		}

		//Can be called on any thread,but essentially delegate to execute in main thread.So there's no race condition
		void SetBackgroundWorkersCount(std::size_t count)
		{
			auto func = [count, this]() 
			{
				std::vector<Worker*> foregroundWorkers;
				std::for_each(m_workerVec.begin(), m_workerVec.end(), [&foregroundWorkers](auto pWorker) 
				{
					if (!pWorker->background)
					{
						foregroundWorkers.push_back(pWorker);
					}
				});

				for (std::size_t i = 0;i < count - (m_workerVec.size() - foregroundWorkers.size());++i)
				{
					foregroundWorkers[i]->background = true;
				}
			};
			if (std::this_thread::get_id() == m_mainThreadId)
			{
				func();
			}
			else
			{
				auto job = AllocateJob(JobType::FOREGROUND, nullptr, func);
				RunJob(job, m_mainThreadId);
			}
		}

		void RunJob(IJob* job, std::thread::id threadId)
		{
			job->SetTargetRunThread(threadId);
			RunJob(job);
		}

		void RunJobOnCurrentThread(IJob* job)
		{
			RunJob(job, GetCurrentThreadId());
		}

		void ShutDown()
		{
			bool expected{ false };
			if (m_shutdown.compare_exchange_strong(expected, true))
			{
				//must lock here because worker thread may modify m_workers at the same time of ShutDown call
				std::lock_guard<std::mutex> lock(m_mutexWorkers);
				std::for_each(m_workers.begin(), m_workers.end(), [](auto pair) {pair.second->running = false; });
			}
		}
	private:
		friend struct Worker;
		struct Worker
		{
			Worker(bool bg) : background(bg)
			{
				//the allocators and queues must be construct in-place ,use some trick code to achieve it
				allocators.emplace(std::piecewise_construct, std::make_tuple(JobType::FOREGROUND), std::make_tuple());
				allocators.emplace(std::piecewise_construct, std::make_tuple(JobType::BACKGROUND), std::make_tuple());
				queues.emplace(std::piecewise_construct, std::make_tuple(JobType::FOREGROUND), std::make_tuple());
				queues.emplace(std::piecewise_construct, std::make_tuple(JobType::BACKGROUND), std::make_tuple());
			}
			//allocators only access through key ,so it doesn't matter if we use map or unordered map.For performance reason just use unordered map
			std::unordered_map<JobType, JobAllocator> allocators;
			//we should always schedule foreground job before background job,so use map to ensure order
			std::map<JobType, WorkStealQueue> queues;
			bool running{ true };
			bool background;
			//increment each time unable to fetch a job from one of the queues
			//if hangCounter reach a certain value,that means the whole system has low payload
			//thus sleep this thread for a while
			std::size_t hangCounter{ 0 };
			static constexpr std::size_t HANG_SLEEP_THRESHOLD{ 10 };
			static constexpr std::size_t MAIN_THREAD_SLEEP_MILLSEC{ 100 };

			void DoRun(bool sleep)
			{
				bool hasJob{ false };
				for (auto it = queues.begin(); it != queues.end();++it)
				{
					if (background && it->first == JobType::BACKGROUND)
					{
						continue;
					}
					auto queue = &it->second;
					auto job = GetJob(it->first, queue);
					if (job)
					{
						hasJob = true;
						hangCounter = 0;
						job->Execute();
					}
				}
				if (!hasJob)
				{
					hangCounter++;
					auto& manager = JobManager::Instance();
					if (sleep && hangCounter >= HANG_SLEEP_THRESHOLD)
					{
						//main thread should not wait
						if (std::this_thread::get_id() != manager.m_mainThreadId)
						{
							manager.m_sleepThreadCount++;
							std::unique_lock<std::mutex> lk(manager.m_mutexWake);
							//Don't consider spurious wakeup because even if that happens,it's no harm to just loop again
							manager.m_cvWake.wait(lk);
							manager.m_sleepThreadCount--;
						}
						else
						{
							//in main thread wait a period of time
							std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_THREAD_SLEEP_MILLSEC));
						}
					}
				}
			}

			IJob* GetJob(JobType type, WorkStealQueue* queue)
			{
				static bool popFirst{ true };
				IJob* job{ nullptr };
				if (popFirst)
				{
					job = GetJob(queue);
					if (!job)
						job = GetJob(type);
				}
				else
				{
					job = GetJob(type);
					if (!job)
						job = GetJob(queue);
				}
				popFirst = !popFirst;
				return job;
			}

			IJob* GetJob(WorkStealQueue* queue)
			{
				return queue->Pop();
			}

			IJob* GetJob(JobType type)
			{
				auto queue = JobManager::Instance().RandomQueue(type, this);
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
					std::lock_guard<std::mutex> lock(system.m_mutexWorkers);
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
					DoRun(true);
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
		std::vector<Worker*> m_workerVec;
		//only meant for initialization
		std::mutex m_mutexWorkers;
		std::mutex m_mutexWake;
		std::condition_variable m_cvWake;
		std::size_t m_sleepThreadCount;
		std::thread::id m_mainThreadId;
		std::atomic<bool> m_shutdown;
		std::function<void()> m_initFunc;
		std::size_t m_workersCount;
	};
}
