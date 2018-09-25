#pragma once
#include <thread>
#include "container.h"
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
			container::unordered_map< std::thread::id, container::vector<ITimer*>> mTimers;
			static constexpr std::size_t sBucketCount = 4096;
		};
	}
}