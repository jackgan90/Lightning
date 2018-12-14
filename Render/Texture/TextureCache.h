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
		class TextureCache : public Foundation::Singleton<TextureCache>
		{
		public:
			~TextureCache();
			ITexture* GetTexture(const std::string& name);
			bool AddTexture(const std::string& name, ITexture* texture);
			bool RemoveTexture(const std::string& name);
		private:
			friend class Foundation::Singleton<TextureCache>;
			TextureCache();
			Container::UnorderedMap<std::string, ITexture*> mTextures;
		};
	}
}
