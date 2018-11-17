#pragma once
#include "FoundationExportDef.h"
#include "IFileSystem.h"
#include "Singleton.h"

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API FileSystemFactory : public Singleton<FileSystemFactory>
		{
		public:
			IFileSystem* CreateFileSystem();
		};
	}
}
