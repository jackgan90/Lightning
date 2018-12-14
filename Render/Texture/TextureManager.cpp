#include <mutex>
#include <cassert>
#include "TextureManager.h"

namespace
{
	std::mutex TextureCacheMutex;
}

namespace Lightning
{
	namespace Render
	{
		TextureManager::TextureManager()
		{

		}

		TextureManager::~TextureManager()
		{
			for (auto it = mTextures.begin(); it != mTextures.end(); ++it)
			{
				it->second->Release();
			}
		}

		ITexture* TextureManager::GetTexture(const std::string& name)
		{
			std::lock_guard<std::mutex> lock(TextureCacheMutex);
			auto it = mTextures.find(name);
			if (it != mTextures.end())
				return it->second;
			return nullptr;
		}

		bool TextureManager::CacheTexture(const std::string& name, ITexture* texture)
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

		bool TextureManager::RemoveTexture(const std::string& name)
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