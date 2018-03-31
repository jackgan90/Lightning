#pragma once
#include <functional>
#include <thread>
#include <atomic>
#include <tuple>
#include <cstdint>
#include <cassert>
#define JOB_ASSERT

#define INVALID_JOB_HANDLE static_cast<std::uint64_t>(-1)

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

	enum class JobType
	{
		//each job type has its own allocation heap and work queue to prevent from race condition
		//foreground job should be quick task,like render,physics update,animation etc
		//background job should be task that runs a period of time such as asset streaming serialization/deserialization
		FOREGROUND,
		BACKGROUND
	};

	class IJob
	{
	public:
		virtual ~IJob() = default;
		virtual void Execute() = 0;
		virtual void Finish() = 0;
		virtual bool HasCompleted() = 0;
		virtual JobType GetType() = 0;
	};

	using JobHandle = std::uint64_t;

	class Job : public IJob
	{
		friend class JobManager;
	public:
		Job(JobType type, IJob* parent):
			m_type(type),
			m_parent(parent),
			m_unfinishedJobs(1),
			m_hasTargetRunThread(false),
			m_hasCompleted(false)
#ifdef JOB_ASSERT
			, m_executeCount(0)
#endif
		{
			//Don't check parent job's finish status.Specify parent job while the job may potentially be completed
			//is an undefined behavior.Users should ensure parent is not completed when child job is created
			if (m_parent)
			{
				auto pParent = static_cast<Job*>(m_parent);
				std::int32_t expected{ 0 };
				//pParent now may or may not be complete here.If it is complete,m_unfinishedJobs should be 0.swap it with an invalid
				//job count,there for Finish cannot be called twice.
				pParent->m_unfinishedJobs.compare_exchange_strong(expected, INVALID_JOB_COUNT, std::memory_order_relaxed);
				pParent->m_unfinishedJobs.fetch_add(1, std::memory_order_release);
			}
		}
		Job(const Job& job) = delete;
		Job& operator=(const Job& job) = delete;

		bool HasCompleted()override
		{
			return m_hasCompleted;
		}

		JobType GetType()override { return m_type; }

	protected:
		void SetTargetRunThread(std::thread::id id)
		{
			m_targetRunThreadId = id;
			m_hasTargetRunThread = true;
		}

		std::thread::id GetTargetRunThread()
		{
			return m_targetRunThreadId;
		}

		bool HasTargetRunThread()const
		{
			return m_hasTargetRunThread;
		}

		JobType m_type;
		IJob* m_parent;
		bool m_hasTargetRunThread;
		std::thread::id m_targetRunThreadId;
		std::atomic<std::int32_t> m_unfinishedJobs;
		//why don't we just compare m_unfinishedJobs with 0 to indicate completeness?
		//Because after the job is done,there are potentially other jobs specifying this job as 
		//their parent.It's no harm to increment m_unfinishedJobs if the job is complete.But it
		//makes this value not accurately reflect the complete status. 
		bool m_hasCompleted;
#ifdef JOB_ASSERT
		std::atomic<std::size_t> m_executeCount;
#endif
		static constexpr std::int32_t INVALID_JOB_COUNT = -(0x3f << 24 | 0xff << 16 | 0xff << 8 | 0xff);
	};

	template<typename Function, typename Tuple>
	class JobImpl : public Job
	{
	private:
		friend class JobAllocator;
		template<typename F, typename A>
		JobImpl(JobType type, IJob* parent, std::atomic<std::size_t>& counter, F&& func, A&& args) :
			Job(type, parent),
			m_payload(std::forward<F>(func), std::forward<A>(args)),
			m_counter(counter)
		{
		}
	public:
		void Execute()override
		{
#ifdef JOB_ASSERT
			assert(m_executeCount == 0);
#endif // JOB_ASSERT

			ApplyWithFunc(m_payload.m_func, m_payload.m_args);
#ifdef JOB_ASSERT
			m_executeCount++;
#endif // JOB_ASSERT
			Finish();
		}

		void Finish()override
		{
			if (HasCompleted())
				return;
			//if job schedule runs as expected,Finish should only be called within one thread
			//so m_unfinishedJobs shouldn't be a negative value.
			auto jobsBefore = m_unfinishedJobs.fetch_sub(1, std::memory_order_acquire);
			if (jobsBefore == 1)
			{
				m_hasCompleted = true;
				//After finish execution,m_payload should be destroyed
				m_payload.~Payload();
				if (m_parent)
					m_parent->Finish();
				//Reset memory 
				*(reinterpret_cast<std::uint8_t*>(this) - 1) = std::uint8_t(0);
				m_counter.fetch_add(1, std::memory_order_relaxed);
			}
		}
	private:
		struct Payload
		{
			template<typename F, typename A>
			Payload(F&& func, A&& args): m_func(std::forward<F>(func)), m_args(std::forward<A>(args)){}
			Function m_func;
			Tuple m_args;
		};
		static constexpr std::size_t MemberSize = sizeof(Job) + sizeof(Payload);
		//std::hardware_destructive_interference_size is defined in c++17,VS2015 doesn't support it
		static constexpr std::size_t CacheLineSize = 64;
		Payload m_payload;
		std::atomic<std::size_t>& m_counter;
		std::uint8_t m_padding[CacheLineSize - MemberSize % CacheLineSize];
	};
}
