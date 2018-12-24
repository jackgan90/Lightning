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
			//InitLoaderIOThreadID must be called by Loader IO thread procedure to let environment know
			//the thread id.
			virtual void INTERFACECALL InitLoaderIOThreadID() = 0;
			virtual bool INTERFACECALL IsInLoaderIOThread()const = 0;
		};
	}
}