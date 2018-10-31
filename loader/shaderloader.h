#pragma once
#include "baseloader.h"

namespace Lightning
{
	namespace Loader
	{
		class LIGHTNING_LOADER_API ShaderLoader : public BaseLoader
		{
		public:
			ShaderLoader();
		protected:
			void* ConstructObject(const Foundation::SharedFilePtr& file, char* buffer)override;
		};
	}
}