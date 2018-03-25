#pragma once
#include <functional>
#include <atomic>
#include <tuple>
#include <cstdint>
#include <cassert>
#define JOB_ASSERT

namespace JobSystem
{
	template<int N>
	struct ApplyFunc
	{
		template<typename Function, typename Tuple, typename... Args>
		static inline auto Apply(Function&& f, Tuple&& t, Args&&... args)		{
			return ApplyFunc<N - 1>::Apply(std::forward<Function>(f),
				std::forward<Tuple>(t), std::get<N-1>(std::forward<Tuple>(t)), std::forward<Args>(args)...);
		}
	};

	template<>
	struct ApplyFunc<0>
	{
		template<typename Function, typename Tuple, typename... Args>
		static inline auto Apply(Function&& f, Tuple&&, Args&&... args)
		{
			return (std::forward<Function>(f))(std::forward<Args>(args)...);
		}
	};

	template<typename Function, typename Tuple>
	auto ApplyWithFunc(Function&& f, Tuple&& t)
	{
		return ApplyFunc<std::tuple_size<typename std::decay<Tuple>::type>::value>::Apply
				(std::forward<Function>(f), std::forward<Tuple>(t));
	}

	class IJob
	{
	public:
		virtual ~IJob() = default;
		virtual void Execute() = 0;
		virtual void Finish() = 0;
		virtual void IncrementCounter() = 0;
		virtual bool HasCompleted() = 0;
	};

	template<typename Function, typename Tuple>
	class Job : public IJob
	{
	private:
		friend class JobAllocator;
		template<typename _Function, typename _Tuple>
		Job(IJob* parent, _Function&& func, _Tuple&& args):
			m_parent(parent), 
			m_func(std::forward<_Function>(func)), 
			m_args(std::forward<_Tuple>(args)), 
			m_unfinishedJobs(1)
#ifdef JOB_ASSERT
			, m_executeCount(0)
#endif
		{
			//Don't check parent job's finish status.Specify parent job while the job may potentially be completed
			//is an undefined behavior.Users should ensure parent is not completed when child job is created
			if (m_parent)
			{
#ifdef JOB_ASSERT
				assert(!m_parent->HasCompleted());
#endif
				m_parent->IncrementCounter();
			}
		}
	public:
		void Execute()override
		{
#ifdef JOB_ASSERT
			assert(m_executeCount == 0);
#endif // JOB_ASSERT

			ApplyWithFunc(m_func, m_args);
			Finish();
		}

		void Finish()override
		{
			//if job schedule runs as expected,Finish should only be called within one thread
			//so m_unfinishedJobs shouldn't be a negative value.
			m_unfinishedJobs--;
			if (HasCompleted())
			{
				if (m_parent)
					m_parent->Finish();
			}
		}
		void IncrementCounter()override
		{
			m_unfinishedJobs++;
		}

		bool HasCompleted()override
		{
			return m_unfinishedJobs == 0;
		}

		Job(const Job<Function, Tuple>& job) = delete;
		Job& operator=(const Job<Function, Tuple>& job) = delete;
	private:
		IJob* m_parent;
		Function m_func;
		Tuple m_args;
		std::atomic<std::int32_t> m_unfinishedJobs;
#ifdef JOB_ASSERT
		std::atomic<std::size_t> m_executeCount;
#endif
		static constexpr std::size_t MemberSize = \
			sizeof(IJob*) + sizeof(Function) + sizeof(Tuple) + sizeof(std::atomic<std::int32_t>)
#ifdef JOB_ASSERT
			+ sizeof(std::atomic<std::size_t>)
#endif
			;
		//std::hardware_destructive_interference_size is defined in c++17,VS2015 doesn't support it
		static constexpr std::size_t CacheLineSize = 64;
		std::uint8_t m_padding[CacheLineSize - MemberSize % CacheLineSize];
	};
}
