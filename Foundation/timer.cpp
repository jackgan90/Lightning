#include <cassert>
#include <algorithm>
#include "timer.h"

namespace Lightning
{
	namespace Foundation
	{
		using namespace std::chrono;
		WheelTimer::WheelTimer(std::size_t wheelSlots, std::size_t resolution):
			mResolution(resolution), mBucketCursor(0), mLoopRound(0), mNextTaskID(1),mBucketCount(wheelSlots),
			mIsIterating(false)
		{
			assert(wheelSlots > 0 && "Bucket count must be positive!");
			mTasks.resize(wheelSlots);
		}

		void WheelTimer::Start()
		{
			mLastTickTime = high_resolution_clock::now();
		}

		void WheelTimer::Tick()
		{
			struct TaskInfo
			{
				std::size_t id;
				std::size_t interval;
				std::function<void()> func;
			};
			static Container::Vector<TaskInfo> repeatedTasks;
			auto now = high_resolution_clock::now();
			auto elapsedTime = duration_cast<milliseconds>(now - mLastTickTime);
			if (std::size_t(elapsedTime.count()) < mResolution)
				return;

			auto& taskList = mTasks[mBucketCursor];
			repeatedTasks.clear();
			mIsIterating = true;
			for (auto it = taskList.begin();it != taskList.end();)
			{
				if (it->loopRound == mLoopRound)
				{
					auto taskID = it->taskID;
					it->func();
					if (mDeletedTasks.find(taskID) == mDeletedTasks.end() && it->repeatInterval > 0)
						repeatedTasks.push_back({ it->taskID, it->repeatInterval, it->func });
					mIDToTasks.erase(it->taskID);
					it = taskList.erase(it);
					continue;
				}
				++it;
			}
			mDeletedTasks.clear();
			mIsIterating = false;
			for (const auto& task : repeatedTasks)
			{
				AddTaskInternal(TimerTaskType::REPEAT, task.interval, task.interval, task.func, task.id);
			}

			if (mBucketCursor == mBucketCount - 1)
				mLoopRound += 1;
			mBucketCursor = (mBucketCursor + 1) % mBucketCount;
			mLastTickTime = now;
		}

		std::size_t WheelTimer::AddTask(TimerTaskType type, std::size_t delay,
			std::size_t repeatInterval, std::function<void()> func)
		{
			delay = std::max(mResolution, delay);
			repeatInterval = std::max(mResolution, repeatInterval);
			auto taskID = AddTaskInternal(type, delay, repeatInterval, func, mNextTaskID);
			++mNextTaskID;
			return taskID;
		}

		std::size_t WheelTimer::AddTaskInternal(TimerTaskType type, std::size_t delay,
			std::size_t repeatInterval, std::function<void()> func, std::size_t taskID)
		{
			if (type == TimerTaskType::ONE_SHOT)
				repeatInterval = 0;
			else
				assert(repeatInterval > 0 && "repeat timer must specify a positive repeat interval!");
			std::size_t cursor, loopRound;
			CalculateCursorAndLoopRound(delay, cursor, loopRound);
			mTasks[cursor].emplace_back(taskID, cursor, repeatInterval, loopRound, func);
			auto *pTask = &mTasks[cursor].back();
			mIDToTasks[taskID] = pTask;

			return taskID;
		}

		void WheelTimer::CalculateCursorAndLoopRound(std::size_t delay, std::size_t& cursor, std::size_t& loopRound)
		{
			cursor = (mBucketCursor + delay / mResolution) % mBucketCount;
			loopRound = mLoopRound + (mBucketCursor + delay / mResolution) / mBucketCount;
		}

		bool WheelTimer::RemoveTask(std::size_t task_id)
		{
			auto it = mIDToTasks.find(task_id);
			if (it == mIDToTasks.end())
				return false;
			if (mIsIterating)
			{
				mDeletedTasks.emplace(task_id);
				return true;
			}
			auto pTask = it->second;
			mTasks[pTask->cursor].remove_if([pTask](const TimerTask& task) {return pTask == &task; });
			mIDToTasks.erase(it);
			return true;
		}
	}
}