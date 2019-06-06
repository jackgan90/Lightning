#pragma once
#include "IDevice.h"
#include "IShader.h"
#include "ISerializer.h"
#include "IShaderMacros.h"

namespace Lightning
{
	namespace Render
	{
		class ShaderSerializer : public Loading::ISerializer
		{
		public:
			ShaderSerializer(ShaderType type, const std::string& path, 
				const std::shared_ptr<IShaderMacros>& macros, ResourceAsyncCallback<IShader> callback);
			~ShaderSerializer()override;
			std::shared_ptr<ISerializeBuffer> Serialize()override;
			void Deserialize(Foundation::IFile* file, const std::shared_ptr<Loading::ISerializeBuffer>& buffer)override;
		private:
			ShaderType mType;
			std::string mPath;
			std::shared_ptr<IShaderMacros> mMacros;
			ResourceAsyncCallback<IShader> mFinishCallback;
			std::shared_ptr<IShader> mShader;
		};
	}
}