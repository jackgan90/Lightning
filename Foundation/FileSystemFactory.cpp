#include <memory>
#include "FileSystemFactory.h"
#include "FileSystem.h"

namespace Lightning
{
	namespace Foundation
	{
		std::unique_ptr<IFileSystem> FileSystemFactory::CreateFileSystem()
		{
			return std::make_unique<GeneralFileSystem>();
		}
	}
}