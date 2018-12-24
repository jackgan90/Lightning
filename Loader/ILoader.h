#pragma once
#include <string>
#include "ISerializer.h"

namespace Lightning
{
	namespace Loading
	{
		struct ILoader
		{
			virtual INTERFACECALL ~ILoader() = default;
			virtual void INTERFACECALL Finalize() = 0;
			virtual void INTERFACECALL Load(const char* path, ISerializer* ser) = 0;
		};
	}
}