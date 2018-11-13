#include <cassert>
#include "device.h"
#include "Loader.h"
#include "serializers/shaderserializer.h"
#include "serializers/textureserializer.h"

namespace Lightning
{
	namespace Render
	{
		using Loading::Loader;
		Device::Device()
		{

		}

		void Device::CreateShaderFromFile(ShaderType type, const std::string& path,
			const ShaderDefine& defineMap, ShaderLoadFinishHandler handler)
		{
			auto ser = new ShaderSerializer(type, path, defineMap, handler);
			Loader::Instance()->Load(path, ser);
		}

		void Device::CreateTextureFromFile(const TextureDescriptor& descriptor, const std::string& path,
			TextureLoadFinishHandler handler)
		{
			auto ser = new TextureSerializer(descriptor, path, handler);
			Loader::Instance()->Load(path, ser);
		}

		SharedShaderPtr Device::GetDefaultShader(ShaderType type)
		{
			auto it = mDefaultShaders.find(type);
			if (it == mDefaultShaders.end())
				return SharedShaderPtr();
			return it->second;
		}
	}
}