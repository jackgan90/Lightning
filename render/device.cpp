#include <cassert>
#include "device.h"
#include "loader.h"
#include "serializers/shaderserializer.h"

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

		SharedShaderPtr Device::GetDefaultShader(ShaderType type)
		{
			auto it = mDefaultShaders.find(type);
			if (it == mDefaultShaders.end())
				return SharedShaderPtr();
			return it->second;
		}
	}
}