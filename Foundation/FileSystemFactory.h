#pragma once
#include "FoundationExportDef.h"
#include "IFileSystem.h"
#include "Singleton.h"

namespace Lightning
{
	namespace Foundation
	{
		class FileSystemFactory : public Singleton<FileSystemFactory>
		{
		public:
			IFileSystem* CreateFileSystem();
		};
	}
}
