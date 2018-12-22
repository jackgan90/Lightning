#pragma once
#include <thread>
#include "Portable.h"

namespace Lightning
{
	namespace Foundation
	{
		struct IEnvironment
		{
			virtual INTERFACECALL ~IEnvironment() = default;
			virtual void INTERFACECALL SetLoaderIOThreadID(std::thread::id) = 0;
			virtual bool INTERFACECALL IsInLoaderIOThread()const = 0;
		};
	}
}