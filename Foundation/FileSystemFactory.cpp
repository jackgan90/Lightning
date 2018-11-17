#include <memory>
#include "FileSystemFactory.h"
#include "FileSystem.h"

namespace Lightning
{
	namespace Foundation
	{
		IFileSystem* FileSystemFactory::CreateFileSystem()
		{
			return NEW_REF_OBJ(GeneralFileSystem);
		}
	}
}