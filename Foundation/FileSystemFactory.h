#pragma once
#include "IFileSystem.h"
#include "Singleton.h"

namespace Lightning
{
	namespace Foundation
	{
		class FileSystemFactory : public Singleton<FileSystemFactory>
		{
		public:
			std::unique_ptr<IFileSystem> CreateFileSystem();
		};
	}
}
