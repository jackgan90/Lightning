#pragma once
#include "iserializer.h"

namespace Lightning
{
	namespace Render
	{
		class TextureSerializer : public Loading::ISerializer
		{
		public:
			void Serialize(char** buffer)override;
			void Deserialize(const Foundation::SharedFilePtr& file, char* buffer)override;
			void Dispose()override;
		};
	}
}