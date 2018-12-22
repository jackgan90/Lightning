#pragma once
#include <string>
#include "IFileSystem.h"
#include "ISerializer.h"

namespace Lightning
{
	namespace Loading
	{
		struct ILoader
		{
			virtual ~ILoader() = default;
			virtual void Finalize() = 0;
			virtual void Load(const char* path, ISerializer* ser) = 0;
		};
	}
}