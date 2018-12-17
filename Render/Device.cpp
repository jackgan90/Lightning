#include <cassert>
#include "Device.h"
#include "IPluginManager.h"
#include "LoaderPlugin.h"
#include "Texture/TextureCache.h"
#include "Serializers/ShaderSerializer.h"
#include "Serializers/TextureSerializer.h"

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginManager* gPluginMgr;
	}
	namespace Render
	{
		Device::Device():mLoader(nullptr)
		{

		}

		Device::~Device()
		{
			for (auto& pair : mDefaultShaders)
			{
				pair.second->Release();
			}
			TextureCache::Instance()->Clear();
		}

		void Device::CreateShaderFromFile(ShaderType type, const std::string& path,
			const ShaderDefine& defineMap, ShaderLoadFinishHandler handler)
		{
			//auto ser = new ShaderSerializer(type, path, defineMap, handler);
			//Loader::Instance()->Load(path, ser);
		}

		void Device::CreateTextureFromFile(const std::string& path, TextureLoadFinishHandler handler)
		{
			auto texture = TextureCache::Instance()->GetObject(path);
			if (texture)
			{
				if (handler)
				{
					handler(texture);
				}
				return;
			}
			auto ser = new TextureSerializer(path, [path, handler](ITexture* texture) {
				if (texture)
					TextureCache::Instance()->AddObject(path, texture);
				if (handler)
				{
					handler(texture);
				}
			});
			if (!mLoader)
			{
				auto loaderPlugin = Plugins::gPluginMgr->GetPlugin<Plugins::LoaderPlugin>("Loader");
				mLoader = loaderPlugin->GetLoader();
			}
			mLoader->Load(path, ser);
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