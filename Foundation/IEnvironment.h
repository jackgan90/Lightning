#pragma once
#include <thread>

namespace Lightning
{
	namespace Foundation
	{
		class IEnvironment
		{
		public:
			virtual ~IEnvironment() = default;
			virtual void SetLoaderIOThreadID(std::thread::id) = 0;
			virtual bool IsInLoaderIOThread()const = 0;
		};
	}
}