#pragma once
#include "job.h"
#define USE_CUSTOM_CONCURRENT_QUEUE

#ifndef USE_CUSTOM_CONCURRENT_QUEUE
#include "concurrentqueue.h"
#endif // !USE_CUSTOM_CONCURRENT_QUEUE


namespace JobSystem
{
	class JobQueue
	{
	public:
		explicit JobQueue::JobQueue(std::size_t size)
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			:m_queue(size) 
#else
			:m_tail(0), m_head(0), m_tailAnchor(0)
#endif
		{}
		JobQueue(const JobQueue&) = delete;
		JobQueue& operator=(const JobQueue&) = delete;
		void Push(IJob* job)
		{
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			auto res = m_queue.enqueue(job);
			assert(res);
#else
			auto slot = m_tailAnchor.fetch_add(1, std::memory_order_release);
			m_queue[slot & Mask] = job;
			++slot;
			auto newSlot = m_tailAnchor.load(std::memory_order_relaxed);
			if (slot == newSlot)
			{
				m_tail.store(slot, std::memory_order_relaxed);
			}
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
			auto head = m_head.load(std::memory_order_relaxed);
			auto tail = m_tail.load(std::memory_order_relaxed);
			if (tail > head)
			{
				auto job = m_queue[head & Mask];
				if (m_head.compare_exchange_strong(head, head + 1, std::memory_order_relaxed))
				{
					return job;
				}
			}
#endif
			return nullptr;
		}
	private:
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
		moodycamel::ConcurrentQueue<IJob*> m_queue;
#else
		static constexpr std::size_t QueueSize = 8192;
		static constexpr std::size_t Mask = QueueSize - 1;
		std::atomic<std::int32_t> m_head;
		std::atomic<std::int32_t> m_tail;
		std::atomic<std::int32_t> m_tailAnchor;
		IJob* m_queue[8192];
#endif
	};
}
