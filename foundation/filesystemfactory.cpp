#include <memory>
#include "filesystemfactory.h"

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
				s_fs = FileSystemPtr(new GeneralFileSystem());
#endif
			}
			return s_fs;
		}
	}
}