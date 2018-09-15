#pragma once
#include "foundationexportdef.h"
#include "filesystem.h"
#include "singleton.h"

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
