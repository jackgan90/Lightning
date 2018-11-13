#include <memory>
#include "FileSystemFactory.h"

namespace Lightning
{
	namespace Foundation
	{
		SharedFileSystemPtr FileSystemFactory::CreateFileSystem()
		{
			return std::make_unique<GeneralFileSystem>();
		}
	}
}