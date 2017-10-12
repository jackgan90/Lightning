#include <memory>
#include "filesystemfactory.h"

namespace LightningGE
{
	namespace Foundation
	{
		SharedFileSystemPtr FileSystemFactory::CreateFileSystem()
		{
			return std::make_unique<GeneralFileSystem>();
		}
	}
}