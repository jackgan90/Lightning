#pragma once
#include "job.h"

namespace JobSystem
{
	//allocator for jobs.Each thread should has its own allocator.
	class JobAllocator
	{
	public:
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
		auto Allocate(JobType type, IJob* parent, Function&& func, Args&&... args)
		{
			using JobType = Job<Function, decltype(std::make_tuple(std::forward<Args>(args)...))>;
			static_assert(sizeof(JobType) <= PoolSize, "job object is too large!");
			if (m_pos + sizeof(JobType) >= PoolSize)
			{
				m_pos = 0;
			}
			auto pJob = new (m_jobPool + m_pos) JobType(type, parent, std::forward<Function>(func), std::forward_as_tuple(std::forward<Args>(args)...));
			m_pos += sizeof(JobType);

			return pJob;
		}
	private:
		static constexpr std::size_t PoolSize = 1024 * 1024;
		std::uint8_t* m_jobPool;
		std::size_t m_pos;
	};
}
