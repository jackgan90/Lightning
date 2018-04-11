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
			m_globalJobQueues = m_queueAllocator.allocate(JOB_TYPE_COUNT);
			m_queueAllocator.construct(&m_globalJobQueues[JOB_TYPE_FOREGROUND], GLOBAL_JOB_QUEUE_SIZE);
			m_queueAllocator.construct(&m_globalJobQueues[JOB_TYPE_BACKGROUND], GLOBAL_JOB_QUEUE_SIZE);
		}
		~JobManager()
		{
			for (std::size_t i = 0; i < JOB_TYPE_COUNT;++i)
			{
				m_queueAllocator.destroy(&m_globalJobQueues[i]);
			}
			m_queueAllocator.deallocate(m_globalJobQueues, JOB_TYPE_COUNT);
		}
		JobManager(const JobManager&) = delete;
		JobManager& operator=(const JobManager&) = delete;
		template<typename Function, typename... Args>
		auto AllocateJob(JobType type, JobHandle parent, Function func, Args&&... args)
		{
			//according to C++ standard,multiple threads read from std::unordered_map is well defined.So there's
			//no problem here
			auto worker = m_workers[GetWorkerIndex(std::this_thread::get_id())];
			return worker->allocators[type].Allocate(type, parent, func, std::forward<Args>(args)...);
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
			m_workers = new Worker*[coreCount + 1];
			m_workersCount = coreCount + 1;
			std::memset(m_workers, 0, sizeof(Worker*)*(coreCount + 1));
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
				worker->boundThreadId = threads.back().get_id();
				m_workers[GetWorkerIndex(worker->boundThreadId)] = worker;
			}

			//Create a worker for main thread
			auto worker = new Worker(background);
			worker->boundThreadId = std::this_thread::get_id();
			m_workers[GetWorkerIndex(worker->boundThreadId)] = worker;
			worker->Run();
			m_cvWakeUp.notify_all();
			std::for_each(threads.begin(), threads.end(), [](auto& thread) {thread.join(); });
			for (std::size_t i = 0;i < m_workersCount;++i)
			{
				delete m_workers[i];
			}
			delete[] m_workers;
			m_workers = nullptr;
		}

		void RunJob(JobHandle handle)
		{
			IJob* job = JobAllocator::JobAddrFromHandle(handle);
			Job* pJob = static_cast<Job*>(job);
			if (pJob->HasTargetRunThread())
			{
				auto worker = m_workers[GetWorkerIndex(pJob->GetTargetRunThread())];
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
			auto worker = m_workers[GetWorkerIndex(std::this_thread::get_id())];
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
			for (std::size_t i = 0;i < m_workersCount;++i)
			{
				auto worker = m_workers[i];
				if (worker->background)
				{
					workerCount++;
				}
			}
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
			if (std::this_thread::get_id() == m_mainThreadId)
			{
				//func();
				ModifyBackgroundWorkersCount(count);
			}
			else
			{
				auto handle = AllocateJob(JobType::JOB_TYPE_FOREGROUND, INVALID_JOB_HANDLE, [this](std::size_t c) {ModifyBackgroundWorkersCount(c); }, count);
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
				for (std::size_t i = 0;i < m_workersCount;++i)
				{
					m_workers[i]->running = false;
				}
			}
		}
	private:
		friend struct Worker;
		struct Worker
		{
			Worker(bool bg) : background(bg)
			{
				allocators = jobAllocator.allocate(JOB_TYPE_COUNT);
				jobAllocator.construct(&allocators[JOB_TYPE_FOREGROUND]);
				jobAllocator.construct(&allocators[JOB_TYPE_BACKGROUND]);
				queues = queueAllocator.allocate(JOB_TYPE_COUNT);
				queueAllocator.construct(&queues[JOB_TYPE_FOREGROUND], LOCAL_JOB_QUEUE_SIZE);
				queueAllocator.construct(&queues[JOB_TYPE_BACKGROUND], LOCAL_JOB_QUEUE_SIZE);
			}
			~Worker()
			{
				for (std::size_t i = 0;i < JOB_TYPE_COUNT;++i)
				{
					jobAllocator.destroy(&allocators[i]);
					queueAllocator.destroy(&queues[i]);
				}
				jobAllocator.deallocate(allocators, JOB_TYPE_COUNT);
				queueAllocator.deallocate(queues, JOB_TYPE_COUNT);
			}
			//allocators only access through key ,so it doesn't matter if we use map or unordered map.For performance reason just use unordered map
			JobAllocator* allocators;
			//we should always schedule foreground job before background job,so use map to ensure order
			JobQueue* queues;
			std::allocator<JobAllocator> jobAllocator;
			std::allocator<JobQueue> queueAllocator;
			//The thread this worker runs.Set by JobManager
			std::thread::id boundThreadId;
			bool running{ true };
			bool background;
			//increment each time unable to fetch a job from one of the queues
			//if hangCounter reach a certain value,that means the whole system has low payload
			//thus sleep this thread for a while
			std::size_t hangCounter{ 0 };
			static constexpr std::size_t HANG_SLEEP_THRESHOLD{ 100 };
			static constexpr std::size_t LOCAL_JOB_QUEUE_SIZE{ 2048 };
			static constexpr std::size_t MAIN_THREAD_SLEEP_MILLSEC{ 100 };

			void DoRun(bool sleep)
			{
				bool hasJob{ false };
				for (std::size_t i = 0;i < JOB_TYPE_COUNT;++i)
				{
					if (!background && i == JOB_TYPE_BACKGROUND)
					{
						continue;
					}
					auto& queue = queues[i];
					auto job = GetJob(static_cast<JobType>(i));
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
				job = queues[type].Pop();
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

		std::size_t GetWorkerIndex(const std::thread::id& threadId)
		{
			std::hash<std::thread::id> hasher;
			auto hashValue = hasher(threadId);
			std::int32_t index = static_cast<std::int32_t>(hashValue % m_workersCount);
			std::int32_t backIndex = index;
			while (backIndex < static_cast<std::int32_t>(m_workersCount) && m_workers[backIndex] && m_workers[backIndex]->boundThreadId != threadId)
			{
				++backIndex;
			}
			if (backIndex < static_cast<std::int32_t>(m_workersCount))
			{
				return backIndex;
			}
			std::int32_t frontIndex = index - 1;
			while (frontIndex >= 0 && m_workers[frontIndex] && m_workers[frontIndex]->boundThreadId != threadId)
			{
				--frontIndex;
			}
			return frontIndex;
		}

		void ModifyBackgroundWorkersCount(std::size_t count)
		{
			std::vector<Worker*> foregroundWorkers;
			std::vector<Worker*> backgroundWorkers;
			if (count > m_workersCount)
				count = m_workersCount;
			for (std::size_t i = 0;i < m_workersCount;++i)
			{
				auto pWorker = m_workers[i];
				if (!pWorker->background)
				{
					foregroundWorkers.push_back(pWorker);
				}
				else
				{
					backgroundWorkers.push_back(pWorker);
				}
			}

			bool changed{ false };
			if (backgroundWorkers.size() < count)
			{
				for (std::size_t i = 0;i < count - backgroundWorkers.size();++i)
				{
					foregroundWorkers[i]->background = true;
					changed = true;
				}
			}
			else if(backgroundWorkers.size() > count)
			{
				for (std::size_t i = 0;i < backgroundWorkers.size() - count;++i)
				{
					backgroundWorkers[i]->background = false;
					changed = true;
				}
			}
			if (changed)
			{
				m_cvWakeUp.notify_all();
			}
		}

		static constexpr std::size_t GLOBAL_JOB_QUEUE_SIZE{ 4096 };
		JobQueue* m_globalJobQueues;
		std::allocator<JobQueue> m_queueAllocator;
		Worker** m_workers;
		std::size_t m_workersCount;
		std::mutex m_mtxWakeUp;
		std::condition_variable m_cvWakeUp;
		std::size_t m_sleepThreadCount;
		std::thread::id m_mainThreadId;
		std::atomic<bool> m_shutdown;
		std::function<void()> m_initFunc;
	};
}
