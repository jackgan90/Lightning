#include "ShaderSerializer.h"
#include "Renderer.h"
#include "Logger.h"

namespace Lightning
{
	namespace Render
	{
		ShaderSerializer::ShaderSerializer(ShaderType type, const std::string& path, 
			const ShaderDefine& defineMap, ShaderLoadFinishHandler handler)
			:mType(type), mPath(path), mDefineMap(defineMap), mFinishHandler(handler)
		{

		}

		void ShaderSerializer::Serialize(char** buffer)
		{

		}

		void ShaderSerializer::Deserialize(Foundation::IFile* file, Loading::ISerializeBuffer* buffer)
		{
			auto device = Renderer::Instance()->GetDevice();
			auto shader = device->CreateShader(mType, mPath, buffer->GetBuffer(), mDefineMap);
			if (mFinishHandler)
			{
				mFinishHandler(shader);
			}
		}

		void ShaderSerializer::Dispose()
		{
			delete this;
		}
	}
}