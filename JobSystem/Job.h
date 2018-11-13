#pragma once
#include <functional>
#include <thread>
#include <atomic>
#include <tuple>
#include <cstdint>
#include <cassert>
#define JOB_ASSERT
#ifdef NDEBUG
#undef JOB_ASSERT
#endif

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

	enum JobType
	{
		//each job type has its own allocation heap and work queue to prevent from race condition
		//foreground job should be quick task,like render,physics update,animation etc
		//background job should be task that runs a period of time such as asset streaming serialization/deserialization
		JOB_TYPE_FOREGROUND,
		JOB_TYPE_BACKGROUND,
		JOB_TYPE_COUNT
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
	struct InvalidJobHandleType
	{
		operator JobHandle()const { return static_cast<JobHandle>(-1); }
	};
	constexpr InvalidJobHandleType INVALID_JOB_HANDLE;

	class Job : public IJob
	{
		friend class JobManager;
	public:
		Job(JobType type, IJob* parent, std::atomic<std::size_t>& counter):
			mType(type),
			mParent(parent),
			mHasTargetRunThread(false),
			mUnfinishedJobs(1),
			mCounter(counter),
			mHasCompleted(false)
#ifdef JOB_ASSERT
			, mExecuteCount(0)
#endif
		{
			//Don't check parent job's finish status.Specify parent job while the job may potentially be completed
			//is an undefined behavior.Users should ensure parent is not completed when child job is created
			if (mParent)
			{
				auto pParent = static_cast<Job*>(mParent);
				std::int32_t expected{ 0 };
				//pParent now may or may not be complete here.If it is complete,mUnfinishedJobs should be 0.swap it with an invalid
				//job count,there for Finish cannot be called twice.
				pParent->mUnfinishedJobs.compare_exchange_strong(expected, INVALID_JOB_COUNT, std::memory_order_relaxed);
				pParent->mUnfinishedJobs.fetch_add(1, std::memory_order_release);
			}
		}
		Job(const Job& job) = delete;
		Job& operator=(const Job& job) = delete;

		bool HasCompleted()override
		{
			return mHasCompleted;
		}

		JobType GetType()override { return mType; }


	protected:
		void SetTargetRunThread(std::thread::id id)
		{
			mTargetRunThreadId = id;
			mHasTargetRunThread = true;
		}

		std::thread::id GetTargetRunThread()
		{
			return mTargetRunThreadId;
		}

		bool HasTargetRunThread()const
		{
			return mHasTargetRunThread;
		}

		JobType mType;
		IJob* mParent;
		bool mHasTargetRunThread;
		std::thread::id mTargetRunThreadId;
		std::atomic<std::int32_t> mUnfinishedJobs;
		std::atomic<std::size_t>& mCounter;
		//why don't we just compare mUnfinishedJobs with 0 to indicate completeness?
		//Because after the job is done,there are potentially other jobs specifying this job as 
		//their parent.It's no harm to increment mUnfinishedJobs if the job is complete.But it
		//makes this value not accurately reflect the complete status. 
		bool mHasCompleted;
#ifdef JOB_ASSERT
		std::atomic<std::size_t> mExecuteCount;
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
			Job(type, parent, counter),
			mPayload(std::move(func), std::forward<A>(args))
		{
		}
	public:
		void Execute()override
		{
#ifdef JOB_ASSERT
			assert(mExecuteCount == 0);
#endif // JOB_ASSERT

			ApplyWithFunc(mPayload.mFunc, mPayload.mArgs);
#ifdef JOB_ASSERT
			mExecuteCount++;
#endif // JOB_ASSERT
			Finish();
		}

		void Finish()override
		{
			if (HasCompleted())
				return;
			//if job schedule runs as expected,Finish should only be called within one thread
			//so mUnfinishedJobs shouldn't be a negative value.
			auto jobsBefore = mUnfinishedJobs.fetch_sub(1, std::memory_order_acquire);
			if (jobsBefore == 1)
			{
				mHasCompleted = true;
				//After finish execution,mPayload should be destroyed
				mPayload.~Payload();
				if (mParent)
					mParent->Finish();
				//Reset memory 
				*(reinterpret_cast<std::uint8_t*>(this) - 1) = std::uint8_t(0);
				mCounter.fetch_add(1, std::memory_order_relaxed);
			}
		}
	private:
		struct Payload
		{
			template<typename F, typename A>
			Payload(F&& func, A&& args): mFunc(std::move(func)), mArgs(std::forward<A>(args)){}
			Function mFunc;
			Tuple mArgs;
		};
		static constexpr std::size_t MemberSize = sizeof(Job) + sizeof(Payload);
		//std::hardware_destructive_interference_size is defined in c++17,VS2015 doesn't support it
		static constexpr std::size_t CacheLineSize = 64;
		Payload mPayload;
		std::uint8_t mPadding[CacheLineSize - MemberSize % CacheLineSize];
	};
}
