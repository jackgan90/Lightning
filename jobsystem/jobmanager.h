#pragma once
#include <thread>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <mutex>
#include <vector>
#include <iostream>
#include <chrono>
#include "joballocator.h"
#include "jobqueue.h"
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
			m_globalJobQueues.emplace(std::piecewise_construct, 
				std::forward_as_tuple(JobType::FOREGROUND), std::make_tuple());
			m_globalJobQueues.emplace(std::piecewise_construct, 
				std::forward_as_tuple(JobType::BACKGROUND), std::make_tuple());
		}
		~JobManager()
		{

		}
		JobManager(const JobManager&) = delete;
		JobManager& operator=(const JobManager&) = delete;
		template<typename Function, typename... Args>
		auto AllocateJob(JobType type, JobHandle parent, Function&& func, Args&&... args)
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
				threads.emplace_back(&Worker::Run, worker);
				m_workers.emplace(threads.back().get_id(), worker);
			}

			//Create a worker for main thread
			auto worker = new Worker(background);
			m_workers.emplace(std::this_thread::get_id(), worker);
			worker->Run();
			m_cvWakeUp.notify_all();
			std::for_each(threads.begin(), threads.end(), [](auto& thread) {thread.join(); });
			std::for_each(m_workers.begin(), m_workers.end(), [](auto pair) {delete pair.second; });
			m_workers.clear();
		}

		void RunJob(JobHandle handle)
		{
			IJob* job = JobAllocator::JobAddrFromHandle(handle);
			Job* pJob = static_cast<Job*>(job);
			if (pJob->HasTargetRunThread())
			{
				auto worker = m_workers[pJob->GetTargetRunThread()];
				worker->queues[job->GetType()].Push(job);
			}
			else
			{
				m_globalJobQueues[job->GetType()].Push(job);
			}
			if (m_sleepThreadCount)
			{
				m_cvWakeUp.notify_one();
			}
		}


		void WaitForCompletion(JobHandle handle)
		{
			auto worker = m_workers[std::this_thread::get_id()];
			while (true)
			{
				IJob* job = JobAllocator::JobAddrFromHandle(handle);
				if (!job || job->HasCompleted())
				{
					break;
				}
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
			return m_workers.size();
		}

		inline std::thread::id GetCurrentThreadId()const
		{
			return std::this_thread::get_id();
		}

		//Can be called on any thread,but essentially delegate to execute in main thread.So there's no race condition
		void SetBackgroundWorkersCount(std::size_t count)
		{
			if (std::this_thread::get_id() == m_mainThreadId)
			{
				//func();
				ModifyBackgroundWorkersCount(count);
			}
			else
			{
				auto handle = AllocateJob(JobType::FOREGROUND, INVALID_JOB_HANDLE, [this](std::size_t c) {ModifyBackgroundWorkersCount(c); }, count);
				RunJobOnMainThread(handle);
			}
		}

		void RunJob(JobHandle handle, std::thread::id threadId)
		{
			IJob* job = JobAllocator::JobAddrFromHandle(handle);
			static_cast<Job*>(job)->SetTargetRunThread(threadId);
			RunJob(handle);
		}

		void RunJobOnCurrentThread(JobHandle handle)
		{
			RunJob(handle, GetCurrentThreadId());
		}

		void RunJobOnMainThread(JobHandle handle)
		{
			RunJob(handle, m_mainThreadId);
		}

		void ShutDown()
		{
			bool expected{ false };
			if (m_shutdown.compare_exchange_strong(expected, true))
			{
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
			std::map<JobType, JobQueue> queues;
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
					auto job = GetJob(it->first);
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
							std::unique_lock<std::mutex> lk(manager.m_mtxWakeUp);
							//Don't consider spurious wakeup because even if that happens,it's no harm to just loop again
							manager.m_cvWakeUp.wait(lk);
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

			IJob* GetJob(JobType type)
			{
				auto& manager = JobManager::Instance();
				IJob* job{ nullptr };
				auto& queue = queues[type];
				job = queue.Pop();
				if (job)
					return job;
				job = manager.m_globalJobQueues[type].Pop();
				return job;
			}


			void Run()
			{
				auto& system = JobManager::Instance();
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

		void ModifyBackgroundWorkersCount(std::size_t count)
		{
			std::vector<Worker*> foregroundWorkers;
			for (auto it = m_workers.begin();it != m_workers.end();++it)
			{
				auto pWorker = it->second;
				if (!pWorker->background)
				{
					foregroundWorkers.push_back(pWorker);
				}
			}

			int bgWorkerCount = static_cast<int>(m_workers.size()) - static_cast<int>(foregroundWorkers.size());
			int addedBgWorkerCount = count - bgWorkerCount;
			for (int i = 0;i < addedBgWorkerCount;++i)
			{
				foregroundWorkers[i]->background = true;
			}
		}


		std::unordered_map<JobType, JobQueue> m_globalJobQueues;
		std::unordered_map<std::thread::id, Worker*> m_workers;
		std::mutex m_mtxWakeUp;
		std::condition_variable m_cvWakeUp;
		std::size_t m_sleepThreadCount;
		std::thread::id m_mainThreadId;
		std::atomic<bool> m_shutdown;
		std::function<void()> m_initFunc;
	};
}
