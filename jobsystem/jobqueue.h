#pragma once
#include "job.h"
#include "concurrentqueue.h"

namespace JobSystem
{
	class JobQueue
	{
	public:
		explicit JobQueue::JobQueue(std::size_t size):m_queue(size) {}
		JobQueue(const JobQueue&) = delete;
		JobQueue& operator=(const JobQueue&) = delete;
		void Push(IJob* job)
		{
			auto res = m_queue.enqueue(job);
			assert(res);
		}
		IJob* Pop()
		{
			if (m_queue.size_approx() > 0)
			{
				IJob* job{ nullptr };
				auto success = m_queue.try_dequeue(job);
				if (!success)
					return nullptr;
				return job;
			}
			return nullptr;
		}
	private:
		moodycamel::ConcurrentQueue<IJob*> m_queue;
	};
}
