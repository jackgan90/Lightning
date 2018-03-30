#pragma once
#include "job.h"

namespace JobSystem
{
	//allocator for jobs.Each thread should has its own allocator.
	class JobAllocator
	{
	public:
		friend class JobManager;
		JobAllocator():
			m_pos(0)
		{
			m_jobPool = new std::uint8_t[PoolSize];
		}
		~JobAllocator()
		{
			delete[] m_jobPool;
		}
		JobAllocator(const JobAllocator&) = delete;
		JobAllocator& operator=(const JobAllocator&) = delete;
		template<typename Function, typename... Args>
		auto Allocate(JobType type, JobHandle parent, Function&& func, Args&&... args)
		{
			using JobType = JobImpl<Function, decltype(std::make_tuple(std::forward<Args>(args)...))>;
			static_assert(sizeof(JobType) <= PoolSize, "job object is too large!");
			if (m_pos + sizeof(JobType) >= PoolSize)
			{
				m_pos = 0;
			}
			IJob* parentJob = JobAddrFromHandle(parent);
			auto pJob = new (m_jobPool + m_pos) JobType(type, parentJob, std::forward<Function>(func), std::make_tuple(std::forward<Args>(args)...));
			m_pos += sizeof(JobType);

			return JobHandleFromAddr(pJob);
		}
	private:
		static IJob* JobAddrFromHandle(JobHandle handle)
		{
			return handle == INVALID_JOB_HANDLE ? nullptr : reinterpret_cast<IJob*>(~handle);
		}

		static JobHandle JobHandleFromAddr(IJob* job)
		{
			return ~reinterpret_cast<JobHandle>(job);
		}

		struct JobPool
		{
			std::uint8_t* buffer;
			std::atomic<std::size_t> head;
			std::size_t rear;
		};
		static constexpr std::size_t PoolSize = 1024 * 1024;
		std::uint8_t* m_jobPool;
		std::size_t m_pos;
	};
}
