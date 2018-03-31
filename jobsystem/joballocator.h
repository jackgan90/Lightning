#pragma once
#include "job.h"

namespace JobSystem
{
	//allocator for jobs.Each thread should has its own allocator.
	class JobAllocator
	{
	public:
		friend class JobManager;
		JobAllocator()
		{
			//Create two primary pools,these pools are not deleted
			//during the lifetime of JobAllocator
			for (std::size_t i = 0;i < 2;++i)
			{
				m_pools.emplace_back(PoolSize, true);
			}
		}

		~JobAllocator()
		{

		}
		JobAllocator(const JobAllocator&) = delete;
		JobAllocator& operator=(const JobAllocator&) = delete;
		template<typename Function, typename... Args>
		auto Allocate(JobType type, JobHandle parent, Function&& func, Args&&... args)
		{
			using JobType = JobImpl<Function, decltype(std::make_tuple(std::forward<Args>(args)...))>;
			static_assert(sizeof(JobType) + 1 <= PoolSize, "job object is too large!");
			JobPool* pool = &m_pools.back();
			if (pool->pos + sizeof(JobType) + 1 > PoolSize)
			{
				pool->locked = true;
				ClearAndAllocatePools();
				pool = &m_pools.back();
			}
			IJob* parentJob = JobAddrFromHandle(parent);
			std::uint8_t* addr = pool->buffer + pool->pos + 1;
			*(addr - 1) = Magic;
			auto pJob = new (addr) JobType(type, parentJob, *pool->finishedJobCount, std::forward<Function>(func), std::make_tuple(std::forward<Args>(args)...));
			pool->pos += sizeof(JobType) + 1;
			pool->allocatedJobCount++;

			return JobHandleFromAddr(pJob);
		}
	private:
		static IJob* JobAddrFromHandle(JobHandle handle)
		{
			if (handle == INVALID_JOB_HANDLE)
				return nullptr;
			std::uint8_t* addr = reinterpret_cast<std::uint8_t*>(~handle);
			if (*(addr - 1) != Magic)
				return nullptr;
			return reinterpret_cast<IJob*>(addr);
		}

		static JobHandle JobHandleFromAddr(IJob* job)
		{
			return ~reinterpret_cast<JobHandle>(job);
		}

		struct JobPool
		{
			JobPool(std::size_t size, bool primary):
				allocatedJobCount(0), 
				pos(0),
				locked(false),
				isPrimary(primary)
			{
				finishedJobCount = new std::atomic<std::size_t>(0);
				buffer = new std::uint8_t[size];
			}
			~JobPool()
			{
				if (buffer)
				{
					delete[] buffer;
					buffer = nullptr;
				}
				if (finishedJobCount)
				{
					delete finishedJobCount;
					finishedJobCount = nullptr;
				}
			}
			JobPool(const JobPool&) = delete;
			JobPool& operator=(const JobPool&) = delete;
			JobPool(JobPool&& pool)noexcept
			{
				buffer = pool.buffer;
				finishedJobCount = pool.finishedJobCount;
				pool.finishedJobCount = nullptr;
				pos = pool.pos;
				locked = pool.locked;
				allocatedJobCount = pool.allocatedJobCount;
				isPrimary = pool.isPrimary;
				pool.buffer = nullptr;
			}
			JobPool& operator=(JobPool&& pool)noexcept
			{
				if (this != &pool)
				{
					buffer = pool.buffer;
					finishedJobCount = pool.finishedJobCount;
					pool.finishedJobCount = nullptr;
					pos = pool.pos;
					locked = pool.locked;
					allocatedJobCount = pool.allocatedJobCount;
					isPrimary = pool.isPrimary;
					pool.buffer = nullptr;
				}
				return *this;
			}
			std::uint8_t* buffer;
			std::atomic<std::size_t>* finishedJobCount;
			std::size_t allocatedJobCount;
			std::size_t pos;
			bool locked;
			bool isPrimary;
		};
		void ClearAndAllocatePools()
		{
			//clear full pool if necessary.The primary pools are not deleted
			std::size_t i{ 0 };
			for (auto it = m_pools.begin();it != m_pools.end();++i)
			{
				auto& pool = *it;
				auto finishedCount = pool.finishedJobCount->load(std::memory_order_relaxed);
				if (pool.locked && finishedCount >= pool.allocatedJobCount)
				{
					if (pool.isPrimary)	//primary pool
					{
						pool.finishedJobCount->store(0, std::memory_order_relaxed);
						pool.locked = false;
						pool.pos = 0;
						pool.allocatedJobCount = 0;
					}
					else
					{
						it = m_pools.erase(it);
						continue;
					}
				}
				++it;
			}
			//try to allocate new pool if requirements are not met
			auto& pool = m_pools.back();
			if (pool.pos != 0)
			{
				//try to swap an empty primary pool to back
				auto itEmpty = m_pools.end();
				for (auto it = m_pools.begin(); it != m_pools.end();++it)
				{
					auto& pool = *it;
					if (!pool.locked && pool.isPrimary && pool.pos == 0 && pool.allocatedJobCount == 0)
					{
						itEmpty = it;
						break;
					}
				}
				if (itEmpty != m_pools.end())
				{
					std::iter_swap(itEmpty, m_pools.begin() + m_pools.size() - 1);
				}
				else
				{
					m_pools.emplace_back(PoolSize, false);
				}
			}
		}
		static constexpr std::size_t PoolSize = 256 * 1024;
		static constexpr std::uint8_t Magic = 0xff;
		std::vector<JobPool> m_pools;
	};
}
