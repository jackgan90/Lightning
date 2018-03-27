#pragma once
#include "job.h"

namespace JobSystem
{
	class WorkStealQueue
	{
	public:
		WorkStealQueue();
		WorkStealQueue(const WorkStealQueue&) = delete;
		WorkStealQueue& operator=(const WorkStealQueue&) = delete;
		//Push and Pop work in the same thread as the caller.So there should be no 
		//race condition
		void Push(IJob* job);
		IJob* Pop();
		//Steal is called by other threads,never be called from the same thread.
		IJob* Steal();
	private:
		static constexpr std::size_t MaxSize = 4096;
		static constexpr std::size_t Mask = MaxSize - 1;
		IJob* m_jobs[MaxSize];
		//although m_rear is only modified by one thread,other threads will read it,and in 32-bit platform a 64-bit assignment/operator+-*/ is not atomic
		std::atomic<std::int64_t> m_rear;
		//m_head will be modified concurrently by several threads, so atomic is required
		std::atomic<std::int64_t> m_head;
	};
}
