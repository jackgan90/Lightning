#include "filesystemfactory.h"
#include <memory>

namespace LightningGE
{
	namespace Foundation
	{
		FileSystemPtr FileSystemFactory::s_fs;
		FileSystemPtr FileSystemFactory::FileSystem()
		{
			if (!s_fs)
			{
#ifdef LIGHTNINGGE_WIN32
			return FileSystemPtr(new GeneralFileSystem());
#endif
			}
			return s_fs;
		}
	}
}