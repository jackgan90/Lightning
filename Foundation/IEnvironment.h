#pragma once
#include <thread>
#include "Portable.h"

namespace Lightning
{
	namespace Foundation
	{
		struct IEnvironment
		{
			virtual ~IEnvironment() = default;
			//the thread id.
			virtual void SetLoaderIOThreadID(std::thread::id id) = 0;
			virtual bool IsInLoaderIOThread()const = 0;
		};
	}
}