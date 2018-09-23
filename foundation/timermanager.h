#pragma once
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "singleton.h"
#include "timer.h"

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API TimerManager : public Singleton<TimerManager>
		{
		public:
			ITimer* CreateTimer(std::size_t resolution);
			ITimer* GetTimer();
		private:
			std::unordered_map < std::thread::id, std::vector<ITimer*>> mTimers;
			static std::mutex sTimerMutex;
			static constexpr std::size_t sBucketCount = 4096;
		};
	}
}