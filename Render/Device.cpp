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

		void Device::CreateShaderFromFile(ShaderType type, const std::string& path, 
			const std::shared_ptr<IShaderMacros>& macros, ResourceAsyncCallback<IShader> callback)
		{
			auto shader = ShaderCache::Instance()->GetShader(type, path, macros.get());
			if (shader)
			{
				if (callback)
				{
					callback(shader);
				}
				return;
			}
			auto serializer = std::make_shared<ShaderSerializer>(type, path, macros, [callback](const std::shared_ptr<IShader>& shader) {
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
			auto serializer = std::make_shared<TextureSerializer>(path, [path, callback](const std::shared_ptr<ITexture>& texture) {
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

		std::shared_ptr<IShader> Device::GetDefaultShader(ShaderType type)
		{
			auto it = mDefaultShaders.find(type);
			if (it == mDefaultShaders.end())
				return nullptr;
			return it->second;
		}
	}
}