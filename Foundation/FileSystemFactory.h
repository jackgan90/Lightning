#pragma once
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
			void DestroyFileSystem(IFileSystem* fs);
		};
	}
}
