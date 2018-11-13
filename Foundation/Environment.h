#pragma once
#include <thread>
#include "FoundationExportDef.h"

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