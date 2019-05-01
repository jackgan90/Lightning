#include <cassert>
#include "Device.h"
#include "IPluginManager.h"
#include "ILoaderPlugin.h"
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
				auto loaderPlugin = Plugins::GetPlugin<Plugins::ILoaderPlugin>(Plugins::gPluginMgr, "Loader");
				mLoader = loaderPlugin->GetLoader();
			}
			return mLoader;
		}

		void Device::CreateShaderFromFile(ShaderType type, const std::string& path, ResourceAsyncCallback<IShader> callback)
		{
			ShaderMacros macros;
			auto shader = ShaderCache::Instance()->GetShader(type, path, &macros);
			if (shader)
			{
				if (callback)
				{
					callback(shader);
				}
				return;
			}
			auto serializer = std::make_shared<ShaderSerializer>(type, path, macros, [callback](IShader* shader) {
					if (shader)
						ShaderCache::Instance()->AddShader(shader);
					if (callback)
					{
						callback(shader);
					}
			});
			auto loader = GetLoader();
			loader->Load(path, serializer);
		}

		void Device::CreateTextureFromFile(const std::string& path, ResourceAsyncCallback<ITexture> callback)
		{
			auto texture = TextureCache::Instance()->GetObject(path);
			if (texture)
			{
				if (callback)
				{
					callback(texture);
				}
				return;
			}
			auto serializer = std::make_shared<TextureSerializer>(path, [path, callback](ITexture* texture) {
					if (texture)
						TextureCache::Instance()->AddObject(path, texture);
					if (callback)
					{
						callback(texture);
					}
			});
			auto loader = GetLoader();
			loader->Load(path, serializer);
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