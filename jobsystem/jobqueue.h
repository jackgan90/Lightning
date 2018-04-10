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
			:m_queue(size) 
#else
			:m_tail(0), m_head(0), m_tailAnchor(0), m_queueSize(size)
#endif
		{
#ifdef USE_CUSTOM_CONCURRENT_QUEUE
			m_queue = new IJob*[size];
#ifdef JOB_ASSERT
			std::memset(m_queue, 0, size * sizeof(IJob*));
#endif
#endif
		}
		JobQueue(const JobQueue&) = delete;
		JobQueue& operator=(const JobQueue&) = delete;
		~JobQueue()
		{
#ifdef USE_CUSTOM_CONCURRENT_QUEUE
			delete[] m_queue;
#endif
		}
		void Push(IJob* job)
		{
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			auto res = m_queue.enqueue(job);
			assert(res);
#else
			auto slot = m_tailAnchor.fetch_add(1, std::memory_order_release);
			AddJobToQueue(slot, job);
			//m_tail.fetch_add(1, std::memory_order_relaxed);
			while (!m_tail.compare_exchange_strong(slot, slot + 1));
#endif
		}
		IJob* Pop()
		{
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			if (m_queue.size_approx() > 0)
			{
				IJob* job{ nullptr };
				auto success = m_queue.try_dequeue(job);
				if (!success)
					return nullptr;
				return job;
			}
#else
			auto tail = m_tail.load(std::memory_order_relaxed);
			if (tail > 0)
			{
				auto head = m_head.load(std::memory_order_relaxed);
				if (tail > head)
				{
					auto job = RemoveJobFromQueue(head);
					if (m_head.compare_exchange_strong(head, head + 1, std::memory_order_relaxed))
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
		moodycamel::ConcurrentQueue<IJob*> m_queue;
#else
		void AddJobToQueue(std::int64_t index, IJob* job)
		{
#ifdef JOB_ASSERT
			assert(job);
#endif
			m_queue[index % m_queueSize] = job;
		}

		IJob* RemoveJobFromQueue(std::int64_t index)
		{
			return m_queue[index % m_queueSize];
		}


		std::atomic<std::int64_t> m_head;
		std::atomic<std::int64_t> m_tail;
		std::atomic<std::int64_t> m_tailAnchor;
		std::uint32_t m_queueSize;
		IJob** m_queue;
#endif
	};
}
