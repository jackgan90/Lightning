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
			TextureSerializer(const std::string path, ResourceAsyncCallback<ITexture> callback);
			~TextureSerializer()override;
			void Serialize(char** buffer)override;
			void Deserialize(Foundation::IFile* file, const std::shared_ptr<Loading::ISerializeBuffer>& buffer)override;
		private:
			std::string mPath;
			ResourceAsyncCallback<ITexture> mFinishCallback;
			ITexture* mTexture;
		};
	}
}