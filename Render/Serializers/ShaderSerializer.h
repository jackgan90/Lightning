#pragma once
#include "IDevice.h"
#include "IShader.h"
#include "ISerializer.h"
#include "ShaderMacros.h"

namespace Lightning
{
	namespace Render
	{
		class ShaderSerializer : public Loading::ISerializer
		{
		public:
			ShaderSerializer(ShaderType type, const std::string& path, 
				const ShaderMacros& macros, ResourceAsyncCallback<IShader> callback);
			~ShaderSerializer()override;
			void Serialize(char** buffer)override;
			void Deserialize(Foundation::IFile* file, const std::shared_ptr<Loading::ISerializeBuffer>& buffer)override;
		private:
			ShaderType mType;
			std::string mPath;
			ShaderMacros mMacros;
			ResourceAsyncCallback<IShader> mFinishCallback;
			std::shared_ptr<IShader> mShader;
		};
	}
}