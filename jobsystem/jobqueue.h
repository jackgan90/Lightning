#pragma once
#include "job.h"
#define USE_CUSTOM_CONCURRENT_QUEUE

#ifndef USE_CUSTOM_CONCURRENT_QUEUE
#include "concurrentqueue.h"
#endif // !USE_CUSTOM_CONCURRENT_QUEUE
#ifdef JOB_ASSERT
#include <cstring>
#endif // JOB_ASSERT



namespace JobSystem
{
	class JobQueue
	{
	public:
		explicit JobQueue::JobQueue(std::size_t size)
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			:mQueue(size) 
#else
			:mTail(0), mHead(0), mTailAnchor(0), mQueueSize(size)
#endif
		{
#ifdef USE_CUSTOM_CONCURRENT_QUEUE
			mQueue = new IJob*[size];
#ifdef JOB_ASSERT
			std::memset(mQueue, 0, size * sizeof(IJob*));
#endif
#endif
		}
		JobQueue(const JobQueue&) = delete;
		JobQueue& operator=(const JobQueue&) = delete;
		~JobQueue()
		{
#ifdef USE_CUSTOM_CONCURRENT_QUEUE
			delete[] mQueue;
#endif
		}
		void Push(IJob* job)
		{
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			auto res = mQueue.enqueue(job);
			assert(res);
#else
			auto slot = mTailAnchor.fetch_add(1, std::memory_order_relaxed);
			AddJobToQueue(slot, job);
			do 
			{
				auto expected = slot;
				if (mTail.compare_exchange_strong(expected, slot + 1, std::memory_order_relaxed))
					break;
			} while (true);
#endif
		}
		IJob* Pop()
		{
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			if (mQueue.size_approx() > 0)
			{
				IJob* job{ nullptr };
				auto success = mQueue.try_dequeue(job);
				if (!success)
					return nullptr;
				return job;
			}
#else
			auto tail = mTail.load(std::memory_order_relaxed);
			if (tail > 0)
			{
				auto head = mHead.load(std::memory_order_relaxed);
				if (tail > head)
				{
					auto job = RemoveJobFromQueue(head);
					if (mHead.compare_exchange_strong(head, head + 1, std::memory_order_relaxed))
					{
#ifdef JOB_ASSERT
						assert(job);
#endif
						return job;
					}
				}
			}
#endif
			return nullptr;
		}
	private:
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
		moodycamel::ConcurrentQueue<IJob*> mQueue;
#else
		void AddJobToQueue(std::int64_t index, IJob* job)
		{
#ifdef JOB_ASSERT
			assert(job);
			//check if this write may overwrite any existing job that has not scheduled yet
			//The overwrite happens when the job queue is considered full (too many jobs crammed in
			//queue and CPU has no ability to finish them in time.Normally applications should decide
			//the queue size before hand)
			auto head = mHead.load(std::memory_order_relaxed);
			auto readGeneration = head / mQueueSize;
			auto readIndex = head % mQueueSize;
			auto writeGeneration = index / mQueueSize;
			auto writeIndex = index % mQueueSize;
			assert(writeGeneration == readGeneration || (writeGeneration - readGeneration == 1 && readIndex >= writeIndex));
#endif
			mQueue[index % mQueueSize] = job;
		}

		IJob* RemoveJobFromQueue(std::int64_t index)
		{
			return mQueue[index % mQueueSize];
		}


		std::atomic<std::int64_t> mHead;
		std::atomic<std::int64_t> mTail;
		std::atomic<std::int64_t> mTailAnchor;
		std::uint32_t mQueueSize;
		IJob** mQueue;
#endif
	};
}
