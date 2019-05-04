#include "ShaderSerializer.h"
#include "Renderer.h"
#include "Logger.h"

namespace Lightning
{
	namespace Render
	{
		ShaderSerializer::ShaderSerializer(ShaderType type, const std::string& path, 
			const std::shared_ptr<IShaderMacros>& macros, ResourceAsyncCallback<IShader> callback)
			:mType(type), mPath(path), mMacros(macros), mFinishCallback(callback)
		{

		}

		ShaderSerializer::~ShaderSerializer()
		{
			if (mFinishCallback)
			{
				mFinishCallback(mShader);
			}
		}

		void ShaderSerializer::Serialize(char** buffer)
		{

		}

		void ShaderSerializer::Deserialize(Foundation::IFile* file, const std::shared_ptr<Loading::ISerializeBuffer>& buffer)
		{
			auto device = Renderer::Instance()->GetDevice();
			mShader = device->CreateShader(mType, mPath, buffer->GetBuffer(), mMacros);
		}
	}
}