#include "ShaderSerializer.h"
#include "Renderer.h"
#include "Logger.h"

namespace Lightning
{
	namespace Render
	{
		ShaderSerializer::ShaderSerializer(ShaderType type, const std::string& path, 
			const ShaderMacros& macros, IShaderCallback* callback)
			:mType(type), mPath(path), mMacros(macros), mFinishCallback(callback)
		{

		}

		void ShaderSerializer::Serialize(char** buffer)
		{

		}

		void ShaderSerializer::Deserialize(Foundation::IFile* file, Loading::ISerializeBuffer* buffer)
		{
			auto device = Renderer::Instance()->GetDevice();
			auto shader = device->CreateShader(mType, mPath, buffer->GetBuffer(), mMacros);
			if (mFinishCallback)
			{
				mFinishCallback->Execute(shader);
			}
		}

		void ShaderSerializer::Dispose()
		{
			delete this;
		}
	}
}