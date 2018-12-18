#include <cassert>
#include "Device.h"
#include "IPluginManager.h"
#include "LoaderPlugin.h"
#include "RenderObjectCache.h"
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
			ShaderCache::Instance()->Clear();
		}

		Loading::ILoader* Device::GetLoader()
		{
			if (!mLoader)
			{
				auto loaderPlugin = Plugins::gPluginMgr->GetPlugin<Plugins::LoaderPlugin>("Loader");
				mLoader = loaderPlugin->GetLoader();
			}
			return mLoader;
		}

		void Device::CreateShaderFromFile(ShaderType type, const std::string& path,
			const ShaderMacros& macros, ShaderLoadFinishHandler handler)
		{
			auto shader = ShaderCache::Instance()->GetShader(type, path, macros);
			if (shader)
			{
				if (handler)
				{
					handler(shader);
				}
				return;
			}
			auto ser = new ShaderSerializer(type, path, macros, [handler](IShader* shader) {
				if (shader)
					ShaderCache::Instance()->AddShader(shader);
				if (handler)
				{
					handler(shader);
				}
			});
			auto loader = GetLoader();
			loader->Load(path, ser);
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
			auto loader = GetLoader();
			loader->Load(path, ser);
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