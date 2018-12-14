#pragma once
#include <string>
#include "Singleton.h"
#include "Container.h"
#include "ITexture.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		class TextureManager : public Foundation::Singleton<TextureManager>
		{
		public:
			~TextureManager();
			ITexture* GetTexture(const std::string& name);
			bool CacheTexture(const std::string& name, ITexture* texture);
			bool RemoveTexture(const std::string& name);
		private:
			friend class Foundation::Singleton<TextureManager>;
			TextureManager();
			Container::UnorderedMap<std::string, ITexture*> mTextures;
		};
	}
}
