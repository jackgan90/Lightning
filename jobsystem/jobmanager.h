#pragma once
#include <thread>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <mutex>
#include <vector>
#include <iostream>
#include <chrono>
#include "JobAllocator.h"
#include "JobQueue.h"
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
		JobManager() : mGlobalJobQueues(nullptr), mShutdown(false), mSleepThreadCount(0)
		{
		}
		~JobManager()
		{
			DestroyGlobalJobQueues();
		}
		JobManager(const JobManager&) = delete;
		JobManager& operator=(const JobManager&) = delete;
		template<typename Function, typename... Args>
		auto AllocateJob(JobType type, JobHandle parent, Function func, Args&&... args)
		{
			//according to C++ standard,multiple threads read from std::unordered_map is well defined.So there's
			//no problem here
			auto worker = mWorkers[GetWorkerIndex(std::this_thread::get_id())];
			return worker->allocators[type].Allocate(type, parent, func, std::forward<Args>(args)...);
		}

		//Only main thread can call run on JobSystem
		void Run(std::function<void()> initFunc, std::uint32_t jobQueueSize)
		{
			//The calling thread is considered to be main thread.
			mGlobalJobQueues = mQueueAllocator.allocate(JOB_TYPE_COUNT);
			mQueueAllocator.construct(&mGlobalJobQueues[JOB_TYPE_FOREGROUND], jobQueueSize);
			mQueueAllocator.construct(&mGlobalJobQueues[JOB_TYPE_BACKGROUND], jobQueueSize);
			mMainThreadId = std::this_thread::get_id();
			mInitFunc = initFunc;
			int coreCount = std::thread::hardware_concurrency();
			if (coreCount > 0)
				coreCount--;		//exclude the calling thread
			mWorkers = new Worker*[coreCount + 1];
			mWorkersCount = coreCount + 1;
			std::memset(mWorkers, 0, sizeof(Worker*)*(coreCount + 1));
			bool background{ true };
			std::vector<std::thread> threads;
			//1/4 threads serve as background worker
			for (int i = 0;i < coreCount;++i)
			{
				//Create per-thread resource for worker threads.The calling thread is also considered a worker thread
				auto worker = new Worker(background, jobQueueSize);
				if (i >= (coreCount + 1) / 4 - 1)
				{
					background = false;
				}
				threads.emplace_back(&Worker::Run, worker);
				worker->boundThreadId = threads.back().get_id();
				mWorkers[GetWorkerIndex(worker->boundThreadId)] = worker;
			}

			//Create a worker for main thread
			auto worker = new Worker(background, jobQueueSize);
			worker->boundThreadId = std::this_thread::get_id();
			mWorkers[GetWorkerIndex(worker->boundThreadId)] = worker;
			worker->Run();
			mWakeUp.notify_all();
			std::for_each(threads.begin(), threads.end(), [](auto& thread) {thread.join(); });
			for (std::size_t i = 0;i < mWorkersCount;++i)
			{
				delete mWorkers[i];
			}
			delete[] mWorkers;
			mWorkers = nullptr;
			DestroyGlobalJobQueues();
		}

		void RunJob(JobHandle handle)
		{
			IJob* job = JobAllocator::JobAddrFromHandle(handle);
			Job* pJob = static_cast<Job*>(job);
			if (pJob->HasTargetRunThread())
			{
				auto worker = mWorkers[GetWorkerIndex(pJob->GetTargetRunThread())];
				worker->queues[job->GetType()].Push(job);
			}
			else
			{
				mGlobalJobQueues[job->GetType()].Push(job);
			}
			if (mSleepThreadCount)
			{
				mWakeUp.notify_one();
			}
		}


		void WaitForCompletion(JobHandle handle)
		{
			auto worker = mWorkers[GetWorkerIndex(std::this_thread::get_id())];
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
			for (std::size_t i = 0;i < mWorkersCount;++i)
			{
				auto worker = mWorkers[i];
				if (worker->background)
				{
					workerCount++;
				}
			}
			return workerCount;
		}

		inline std::size_t GetWorkersCount()const
		{
			return mWorkersCount;
		}

		inline std::thread::id GetCurrentThreadId()const
		{
			return std::this_thread::get_id();
		}

		//Can be called on any thread,but essentially delegate to execute in main thread.So there's no race condition
		void SetBackgroundWorkersCount(std::size_t count)
		{
			if (std::this_thread::get_id() == mMainThreadId)
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
			RunJob(handle, mMainThreadId);
		}

		void ShutDown()
		{
			bool expected{ false };
			if (mShutdown.compare_exchange_strong(expected, true))
			{
				for (std::size_t i = 0;i < mWorkersCount;++i)
				{
					mWorkers[i]->running = false;
				}
			}
		}
	private:
		friend struct Worker;
		struct Worker
		{
			Worker(bool bg, std::uint32_t localJobQueueSize) : background(bg)
			{
				allocators = jobAllocator.allocate(JOB_TYPE_COUNT);
				jobAllocator.construct(&allocators[JOB_TYPE_FOREGROUND]);
				jobAllocator.construct(&allocators[JOB_TYPE_BACKGROUND]);
				queues = queueAllocator.allocate(JOB_TYPE_COUNT);
				queueAllocator.construct(&queues[JOB_TYPE_FOREGROUND], localJobQueueSize);
				queueAllocator.construct(&queues[JOB_TYPE_BACKGROUND], localJobQueueSize);
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
						if (std::this_thread::get_id() != manager.mMainThreadId)
						{
							manager.mSleepThreadCount++;
							std::unique_lock<std::mutex> lk(manager.mMutexWakeUp);
							//Don't consider spurious wakeup because even if that happens,it's no harm to just loop again
							manager.mWakeUp.wait(lk);
							manager.mSleepThreadCount--;
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
				job = manager.mGlobalJobQueues[type].Pop();
				return job;
			}


			void Run()
			{
				auto& system = JobManager::Instance();
				if (std::this_thread::get_id() == JobManager::Instance().mMainThreadId)
				{
					JobManager::Instance().mInitFunc();
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
			std::int32_t index = static_cast<std::int32_t>(hashValue % mWorkersCount);
			std::int32_t backIndex = index;
			while (backIndex < static_cast<std::int32_t>(mWorkersCount) && mWorkers[backIndex] && mWorkers[backIndex]->boundThreadId != threadId)
			{
				++backIndex;
			}
			if (backIndex < static_cast<std::int32_t>(mWorkersCount))
			{
				return backIndex;
			}
			std::int32_t frontIndex = index - 1;
			while (frontIndex >= 0 && mWorkers[frontIndex] && mWorkers[frontIndex]->boundThreadId != threadId)
			{
				--frontIndex;
			}
			return frontIndex;
		}

		void ModifyBackgroundWorkersCount(std::size_t count)
		{
			std::vector<Worker*> foregroundWorkers;
			std::vector<Worker*> backgroundWorkers;
			if (count > mWorkersCount)
				count = mWorkersCount;
			for (std::size_t i = 0;i < mWorkersCount;++i)
			{
				auto pWorker = mWorkers[i];
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
				mWakeUp.notify_all();
			}
		}

		void DestroyGlobalJobQueues()
		{
			if (mGlobalJobQueues)
			{
				for (std::size_t i = 0; i < JOB_TYPE_COUNT;++i)
				{
					mQueueAllocator.destroy(&mGlobalJobQueues[i]);
				}
				mQueueAllocator.deallocate(mGlobalJobQueues, JOB_TYPE_COUNT);
				mGlobalJobQueues = nullptr;
			}
		}

		JobQueue* mGlobalJobQueues;
		std::allocator<JobQueue> mQueueAllocator;
		Worker** mWorkers;
		std::size_t mWorkersCount;
		std::mutex mMutexWakeUp;
		std::condition_variable mWakeUp;
		std::size_t mSleepThreadCount;
		std::thread::id mMainThreadId;
		std::atomic<bool> mShutdown;
		std::function<void()> mInitFunc;
	};
}
