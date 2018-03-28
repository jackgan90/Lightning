#include "workstealqueue.h"

namespace JobSystem
{
	WorkStealQueue::WorkStealQueue():
		m_head(0), m_rear(0)
	{

	}

	void WorkStealQueue::Push(IJob* job)
	{
		//data overwrite is an issue,but we assume it will not happen.....
		auto rear = m_rear.fetch_add(1, std::memory_order_relaxed);
		m_jobs[rear & Mask] = job;
	}

	IJob* WorkStealQueue::Pop()
	{
		//The following 2 lines are super important
		//first decrease rear to make other threads read it as if the rear element is popped successfully
		auto rear = m_rear.fetch_add(-1, std::memory_order_relaxed) - 1;

		auto head = m_head.load(std::memory_order_relaxed);
		if (head <= rear)
		{
			//When this line is hit,there are actually two situations:
			//1.head == rear which means the queue only contains one job and it can either be popped by this thread or be stealt by one of 
			//the other threads
			//2.head != rear which means the queue contains at least 2 jobs.And we already decrement m_rear before Pop successfully returns a job.
			//In this case,the queue is considered "shorter" than it really is.If other threads steal jobs from here on,they only see a queue which
			//has exactly one less than the actual job numbers.So in this case,it's safe to just return the last job because other threads cannot
			//steal it from this thread.
			auto job = m_jobs[rear & Mask];
			if (!CanRunInCurrentThread(job))
			{
				m_rear.fetch_add(1, std::memory_order_relaxed);
				return nullptr;
			}
			if (head != rear)
			{
				return job;
			}
			//we must perform a CAS to compete with other threads 
			if (!m_head.compare_exchange_strong(head, head + 1))
			{
				//compete failed,we cannot pop job,because other thread stealt it 
				job = nullptr;
			}
			//if job is nullptr,we failed to pop the last job,which means other thread has stolen it and the queue is empty
			//so m_head in incremented and became head + 1(it has to be head + 1, because Push cannot be called at the same time Pop is called
			//and we already know that there's only one job left in the queue before compare_exchange_strong) 
			//we make m_rear = m_head to indicate queue empty
			//Otherwise if job is not nullptr, we can pop it and the queue will also become empty.But we succeeded in compare_exchange_strong
			//and m_head in incremented,m_head == head + 1, also make m_rear equals to m_head
			m_rear.store(head + 1, std::memory_order_relaxed);
			return job;
		}
		m_rear.fetch_add(1, std::memory_order_relaxed);
		return nullptr;
	}

	IJob* WorkStealQueue::Steal()
	{
		auto head = m_head.load(std::memory_order_relaxed);
		auto rear = m_rear.load(std::memory_order_relaxed);
		if (rear > head)
		{
			auto job = m_jobs[head & Mask];
			if (!CanRunInCurrentThread(job))
				return nullptr;
			if (m_head.compare_exchange_strong(head, head + 1))
			{
				return job;
			}
		}
		return nullptr;
	}
}