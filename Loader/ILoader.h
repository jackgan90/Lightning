#pragma once
#include <string>
#include "IFileSystem.h"
#include "ISerializer.h"

namespace Lightning
{
	namespace Loading
	{
		class ILoader
		{
		public:
			virtual ~ILoader() = default;
			virtual void Finalize() = 0;
			virtual void Load(const std::string& path, ISerializer* ser) = 0;
		};
	}
}