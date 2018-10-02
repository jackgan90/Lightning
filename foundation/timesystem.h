#pragma once
#include <string>
#include "foundationexportdef.h"

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API Time
		{
		public:
			static std::string GetCurrentTimeString();
			static std::size_t Now();
		};
	}
}

