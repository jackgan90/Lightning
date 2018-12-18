#pragma once
#include "RefObjectCache.h"
#include "Texture/ITexture.h"
#include "IShader.h"

namespace Lightning
{
	namespace Render
	{
		using TextureCache = Foundation::RefObjectCache<ITexture>;

		class ShaderCache : public Foundation::RefObjectCache<IShader>
		{
		public:
			IShader* GetShader(ShaderType type, const std::string& name, const ShaderMacros& macros);
		};
	}
}