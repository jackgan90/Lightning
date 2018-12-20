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

		void Device::CreateShaderFromFile(ShaderType type, const char* const path, IShaderLoadCallback* callback)
		{
			class ShaderLoaded : public IShaderLoadCallback
			{
			public:
				ShaderLoaded(IShaderLoadCallback* callback):mCallback(callback){}
				void operator()(IShader* shader)
				{
					if (shader)
						ShaderCache::Instance()->AddShader(shader);
					if (mCallback)
					{
						mCallback->operator()(shader);
					}
					delete this;
				}
			private:
				IShaderLoadCallback* mCallback;
			};
			ShaderMacros macros;
			auto shader = ShaderCache::Instance()->GetShader(type, path, macros);
			if (shader)
			{
				if (callback)
				{
					callback->operator()(shader);
				}
				return;
			}
			auto ser = new ShaderSerializer(type, path, macros, new ShaderLoaded(callback));
			auto loader = GetLoader();
			loader->Load(path, ser);
		}

		void Device::CreateTextureFromFile(const char* const path, ITextureLoadCallback* callback)
		{
			class TextureLoaded : public ITextureLoadCallback
			{
			public:
				TextureLoaded(const std::string& path, ITextureLoadCallback* callback):mCallback(callback), mPath(path){}
				void operator()(ITexture* texture)
				{
					if (texture)
						TextureCache::Instance()->AddObject(mPath, texture);
					if (mCallback)
					{
						mCallback->operator()(texture);
					}
					delete this;
				}
			private:
				ITextureLoadCallback* mCallback;
				std::string mPath;
			};

			auto texture = TextureCache::Instance()->GetObject(path);
			if (texture)
			{
				if (callback)
				{
					callback->operator()(texture);
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