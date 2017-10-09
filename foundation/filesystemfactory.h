#pragma once
#include "foundationexportdef.h"
#include "filesystem.h"
#include "singleton.h"

namespace LightningGE
{
	namespace Foundation
	{
		class LIGHTNINGGE_FOUNDATION_API FileSystemFactory : public Singleton<FileSystemFactory>
		{
		public:
			IFileSystem<GeneralFileSystem>* FileSystem();
			void Finalize();
		};
	}
}