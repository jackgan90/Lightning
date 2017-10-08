#pragma once
#include "foundationexportdef.h"
#include "filesystem.h"

namespace LightningGE
{
	namespace Foundation
	{
		class LIGHTNINGGE_FOUNDATION_API FileSystemFactory
		{
		public:
			static FileSystemPtr FileSystem();
			static void Finalize();
		private:
			static FileSystemPtr s_fs;
		};
	}
}