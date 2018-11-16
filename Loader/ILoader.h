#pragma once
#include <string>
#include "FileSystem.h"
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
			virtual void SetFileSystem(Foundation::IFileSystem* fs) = 0;
			virtual void Load(const std::string& path, ISerializer* ser) = 0;
		};
	}
}