#include <memory>
#include "filesystemfactory.h"

namespace LightningGE
{
	namespace Foundation
	{
		IFileSystem<GeneralFileSystem>* FileSystemFactory::FileSystem()
		{
			return GeneralFileSystem::Instance();
		}

		void FileSystemFactory::Finalize()
		{

		}

	}
}