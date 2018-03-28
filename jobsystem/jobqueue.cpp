#include "jobqueue.h"

namespace JobSystem
{
	JobQueue::JobQueue(std::size_t size):m_queue(size)
	{

	}

	void JobQueue::Push(IJob* job)
	{
		assert(m_queue.enqueue(job));
	}

	IJob* JobQueue::Pop()
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
}