#include <memory>
#include "filesystemfactory.h"

namespace LightningGE
{
	namespace Foundation
	{
		FileSystemPtr FileSystemFactory::CreateFileSystem()
		{
			return std::make_shared<GeneralFileSystem>();
		}
	}
}