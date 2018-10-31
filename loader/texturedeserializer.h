#pragma once
#include "ideserializer.h"

namespace Lightning
{
	namespace Loading
	{
		class LIGHTNING_LOADER_API TextureDeserializer : public IDeserializer
		{
		public:
			void Deserialize(const Foundation::SharedFilePtr& file, char* buffer, 
				void** resource, bool& auto_delete_resource)override;
		};
	}
}