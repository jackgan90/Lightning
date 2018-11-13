#include <memory>
#include "filesystemfactory.h"

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