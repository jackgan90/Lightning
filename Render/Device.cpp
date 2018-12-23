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

		void Device::CreateShaderFromFile(ShaderType type, const char* const path, IShaderCallback* callback)
		{
			struct ShaderLoaded : IShaderCallback
			{
				ShaderLoaded(IShaderCallback* callback):mCallback(callback){}
				void Execute(IShader* shader)override
				{
					if (shader)
						ShaderCache::Instance()->AddShader(shader);
					if (mCallback)
					{
						mCallback->Execute(shader);
					}
					delete this;
				}

				IShaderCallback* mCallback;
			};
			ShaderMacros macros;
			auto shader = ShaderCache::Instance()->GetShader(type, path, &macros);
			if (shader)
			{
				if (callback)
				{
					callback->Execute(shader);
				}
				return;
			}
			auto ser = new ShaderSerializer(type, path, macros, new ShaderLoaded(callback));
			auto loader = GetLoader();
			loader->Load(path, ser);
		}

		void Device::CreateTextureFromFile(const char* const path, ITextureCallback* callback)
		{
			struct TextureLoaded : ITextureCallback
			{
				TextureLoaded(const std::string& path, ITextureCallback* callback):mCallback(callback), mPath(path){}
				void Execute(ITexture* texture)override
				{
					if (texture)
						TextureCache::Instance()->AddObject(mPath, texture);
					if (mCallback)
					{
						mCallback->Execute(texture);
					}
					delete this;
				}

				ITextureCallback* mCallback;
				std::string mPath;
			};

			auto texture = TextureCache::Instance()->GetObject(path);
			if (texture)
			{
				if (callback)
				{
					callback->Execute(texture);
				}
				return;
			}
			auto ser = new TextureSerializer(path, new TextureLoaded(path, callback));
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