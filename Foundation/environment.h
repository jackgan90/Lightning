#pragma once
#include <thread>
#include "foundationexportdef.h"

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API Environment
		{
		public:
			static std::thread::id LoaderIOThreadID;
			static bool IsInLoaderIOThread();
		};
	}
}