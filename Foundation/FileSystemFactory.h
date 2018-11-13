#pragma once
#include "FoundationExportDef.h"
#include "FileSystem.h"
#include "Singleton.h"

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API FileSystemFactory : public Singleton<FileSystemFactory>
		{
		public:
			SharedFileSystemPtr CreateFileSystem();
		};
	}
}
