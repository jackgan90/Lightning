#include "filesystemfactory.h"
#include <memory>

namespace LightningGE
{
	namespace Foundation
	{
		FileSystemPtr FileSystemFactory::FileSystem()
		{
#ifdef LIGHTNINGGE_WIN32
			return std::make_shared<GeneralFileSystem>();
#endif
			return nullptr;
		}
	}
}