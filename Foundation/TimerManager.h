#pragma once
#include <thread>
#include <unordered_map>
#include "Singleton.h"
#include "Timer.h"

namespace Lightning
{
	namespace Foundation
	{
		class TimerManager : public Singleton<TimerManager>
		{
		public:
			ITimer* CreateTimer(std::size_t resolution);
			ITimer* GetTimer();
		private:
			std::unordered_map< std::thread::id, std::vector<ITimer*>> mTimers;
			static constexpr std::size_t sBucketCount = 4096;
		};
	}
}