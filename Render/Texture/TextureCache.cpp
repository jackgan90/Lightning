#include <mutex>
#include <cassert>
#include "TextureCache.h"

namespace
{
	std::mutex TextureCacheMutex;
}

namespace Lightning
{
	namespace Render
	{
		TextureCache::TextureCache()
		{

		}

		TextureCache::~TextureCache()
		{
			for (auto it = mTextures.begin(); it != mTextures.end(); ++it)
			{
				it->second->Release();
			}
		}

		ITexture* TextureCache::GetTexture(const std::string& name)
		{
			std::lock_guard<std::mutex> lock(TextureCacheMutex);
			auto it = mTextures.find(name);
			if (it != mTextures.end())
				return it->second;
			return nullptr;
		}

		bool TextureCache::CacheTexture(const std::string& name, ITexture* texture)
		{
			assert(texture != nullptr && "Try to cache a null texture!");
			std::lock_guard<std::mutex> lock(TextureCacheMutex);
			if (mTextures.find(name) == mTextures.end())
			{
				texture->AddRef();
				mTextures.emplace(name, texture);
				return true;
			}
			return false;
		}

		bool TextureCache::RemoveTexture(const std::string& name)
		{
			std::lock_guard<std::mutex> lock(TextureCacheMutex);
			auto it = mTextures.find(name);
			if (it != mTextures.end())
			{
				it->second->Release();
				mTextures.erase(it);
				return true;
			}
			return false;
		}
	}
}