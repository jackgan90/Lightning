#pragma once
#include "ISerializer.h"
#include "IDevice.h"

namespace Lightning
{
	namespace Render
	{
		class TextureSerializer : public Loading::ISerializer
		{
		public:
			TextureSerializer(const std::string path, TextureLoadFinishHandler finishHandler);
			void Serialize(char** buffer)override;
			void Deserialize(Foundation::IFile* file, Loading::ISerializeBuffer* buffer)override;
			void Dispose()override;
		private:
			std::string mPath;
			TextureLoadFinishHandler mFinishHandler;
		};
	}
}