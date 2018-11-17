#pragma once
#include <string>
#include "FoundationExportDef.h"

namespace Lightning
{
	namespace Foundation
	{
		class Time
		{
		public:
			//static std::string GetCurrentTimeString();
			static long long Now();
		};
	}
}

