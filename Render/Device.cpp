#include <cassert>
#include "Device.h"
#include "Loader.h"
#include "serializers/ShaderSerializer.h"
#include "serializers/TextureSerializer.h"

namespace Lightning
{
	namespace Render
	{
		using Loading::Loader;
		Device::~Device()
		{
			for (auto& pair : mDefaultShaders)
			{
				pair.second->Release();
			}
		}

		void Device::CreateShaderFromFile(ShaderType type, const std::string& path,
			const ShaderDefine& defineMap, ShaderLoadFinishHandler handler)
		{
			//auto ser = new ShaderSerializer(type, path, defineMap, handler);
			//Loader::Instance()->Load(path, ser);
		}

		void Device::CreateTextureFromFile(const TextureDescriptor& descriptor, const std::string& path,
			TextureLoadFinishHandler handler)
		{
			//auto ser = new TextureSerializer(descriptor, path, handler);
			//Loader::Instance()->Load(path, ser);
		}

		IShader* Device::GetDefaultShader(ShaderType type)
		{
			auto it = mDefaultShaders.find(type);
			if (it == mDefaultShaders.end())
				return nullptr;
			return it->second;
		}
	}
}