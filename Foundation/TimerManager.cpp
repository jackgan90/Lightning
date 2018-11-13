#include "TimerManager.h"

namespace Lightning
{
	namespace Foundation
	{
		ITimer* TimerManager::CreateTimer(std::size_t resolution)
		{
			auto threadId = std::this_thread::get_id();
			auto it = mTimers.find(threadId);
			if (it == mTimers.end())
			{
				mTimers.emplace(std::piecewise_construct, std::make_tuple(threadId), std::make_tuple());
				it = mTimers.find(threadId);
			}
			ITimer *timer = new WheelTimer(sBucketCount, resolution);
			it->second.push_back(timer);
			return timer;
		}

		ITimer* TimerManager::GetTimer()
		{
			auto threadId = std::this_thread::get_id();
			auto it = mTimers.find(threadId);
			if (it == mTimers.end())
				return nullptr;
			if (it->second.empty())
				return nullptr;
			return it->second[0];
		}
	}
}