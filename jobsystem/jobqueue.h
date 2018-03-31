#pragma once
#include "job.h"
#include "concurrentqueue.h"

namespace JobSystem
{
	class JobQueue
	{
	public:
		JobQueue::JobQueue(std::size_t size = GLOBAL_JOB_QUEUE_SIZE):m_queue(size) {}
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
		static constexpr std::size_t GLOBAL_JOB_QUEUE_SIZE{ 8192 };
		moodycamel::ConcurrentQueue<IJob*> m_queue;
	};
}
