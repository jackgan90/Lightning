#pragma once
#include "baseloader.h"

namespace Lightning
{
	namespace Loader
	{
		class LIGHTNING_LOADER_API TextureLoader : public BaseLoader
		{
		public:
			TextureLoader();
		protected:
			void* ConstructObject(const Foundation::SharedFilePtr& file, char* buffer)override;
		};
	}
}