#include <memory>
#include "FileSystemFactory.h"
#include "FileSystem.h"

namespace Lightning
{
	namespace Foundation
	{
		IFileSystem* FileSystemFactory::CreateFileSystem()
		{
			return new GeneralFileSystem;
		}

		void FileSystemFactory::DestroyFileSystem(IFileSystem* fs)
		{
			delete fs;
		}
	}
}