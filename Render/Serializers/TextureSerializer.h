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
			TextureSerializer(const std::string path, ITextureCallback* callback);
			INTERFACECALL ~TextureSerializer()override;
			void INTERFACECALL Serialize(char** buffer)override;
			void INTERFACECALL Deserialize(Foundation::IFile* file, Loading::ISerializeBuffer* buffer)override;
			void INTERFACECALL Dispose()override;
		private:
			std::string mPath;
			ITextureCallback* mFinishCallback;
			ITexture* mTexture;
		};
	}
}