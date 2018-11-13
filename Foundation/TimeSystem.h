#pragma once
#include <string>
#include "FoundationExportDef.h"

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API Time
		{
		public:
			//static std::string GetCurrentTimeString();
			static long long Now();
		};
	}
}

