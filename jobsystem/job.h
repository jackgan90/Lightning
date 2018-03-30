#pragma once
#include <functional>
#include <thread>
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

	class Job : public IJob
	{
		friend class JobManager;
	public:
		Job(JobType type, IJob* parent):
			m_type(type),
			m_parent(parent),
			m_unfinishedJobs(1),
			m_hasTargetRunThread(false)
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
				static_cast<Job*>(m_parent)->m_unfinishedJobs++;
			}
		}
		Job(const Job& job) = delete;
		Job& operator=(const Job& job) = delete;

		bool HasCompleted()override
		{
			return m_unfinishedJobs <= 0;
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
#ifdef JOB_ASSERT
		std::atomic<std::size_t> m_executeCount;
#endif
	};

	template<typename Function, typename Tuple>
	class JobImpl : public Job
	{
	private:
		friend class JobAllocator;
		template<typename F, typename A>
		JobImpl(JobType type, IJob* parent, F&& func, A&& args):
			Job(type, parent),
			m_payload(std::forward<F>(func), std::forward<A>(args))
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
			//if job schedule runs as expected,Finish should only be called within one thread
			//so m_unfinishedJobs shouldn't be a negative value.
			m_unfinishedJobs--;
			//No concurrent call here,just compare
			if (HasCompleted())	
			{
				//After finish execution,m_payload should be destroyed
				m_payload.~Payload();
				if (m_parent)
					m_parent->Finish();
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
		std::uint8_t m_padding[CacheLineSize - MemberSize % CacheLineSize];
	};
}
