#pragma once
#include "ideserializer.h"

namespace Lightning
{
	namespace Loader
	{
		class LIGHTNING_LOADER_API TextureDeserializer : public IDeserializer
		{
		public:
			void Deserialize(const Foundation::SharedFilePtr& file, char* buffer, void** resource)override;
		};
	}
}