#pragma once
#include "job.h"
#include "concurrentqueue.h"

namespace JobSystem
{
	class JobQueue
	{
	public:
		JobQueue(std::size_t size=GLOBAL_JOB_QUEUE_SIZE);
		JobQueue(const JobQueue&) = delete;
		JobQueue& operator=(const JobQueue&) = delete;
		//Push and Pop work in the same thread as the caller.So there should be no 
		//race condition
		void Push(IJob* job);
		IJob* Pop();
	private:
		static constexpr std::size_t GLOBAL_JOB_QUEUE_SIZE{ 8192 };
		moodycamel::ConcurrentQueue<IJob*> m_queue;
	};
}
