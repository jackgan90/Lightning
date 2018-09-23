#pragma once
#include <functional>
#include <cstdint>
#include <list>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "foundationexportdef.h"

namespace Lightning
{
	namespace Foundation
	{
		enum class TimerTaskType : std::uint8_t
		{
			ONE_SHOT,
			REPEAT,
		};

		class LIGHTNING_FOUNDATION_API ITimer
		{
		public:
			virtual ~ITimer() {};
			virtual void Start() = 0;
			virtual void Tick() = 0;
			virtual std::size_t AddTask(TimerTaskType type, std::size_t delay,
				std::size_t repeatInterval, std::function<void()> func) = 0;
			virtual bool RemoveTask(std::size_t task_id) = 0;
		};

		//simple thread-unsafe wheel timer implementation
		class LIGHTNING_FOUNDATION_API WheelTimer : public ITimer
		{
		public:
			WheelTimer(std::size_t wheelSlots, std::size_t resolution);
			void Start()override;
			void Tick() override;
			std::size_t AddTask(TimerTaskType type, std::size_t delay,
				std::size_t repeatInterval, std::function<void()> func) override;
			bool RemoveTask(std::size_t task_id) override;
		private:
			void CalculateCursorAndLoopRound(std::size_t delay, std::size_t& cursor, std::size_t& loopRound);
			std::size_t AddTaskInternal(TimerTaskType type, std::size_t delay,
				std::size_t repeatInterval, std::function<void()> func, std::size_t taskID = 0);
			struct TimerTask
			{
				TimerTask(std::size_t id, std::size_t c, std::size_t interval, std::size_t lr, std::function<void()> f):
					taskID(id), cursor(c), repeatInterval(interval), loopRound(lr), func(f)
				{
				}
				std::size_t taskID;
				std::size_t cursor;
				std::size_t repeatInterval;
				std::size_t loopRound;
				std::function<void()> func;
			};
			std::vector<std::list<TimerTask>> mTasks;
			std::unordered_map<std::size_t, TimerTask*> mIDToTasks;
			std::size_t mResolution;
			std::size_t mBucketCursor;
			std::size_t mLoopRound;
			std::size_t mNextTaskID;
			std::size_t mBucketCount;
			std::chrono::time_point<std::chrono::high_resolution_clock> mLastTickTime;
		};
	}
}