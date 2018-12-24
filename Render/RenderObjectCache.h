#pragma once
#include "RefObjectCache.h"
#include "Texture/ITexture.h"
#include "IShader.h"

namespace Lightning
{
	namespace Render
	{
		class TextureCache : public Foundation::RefObjectCache<TextureCache, std::string, ITexture>
		{

		};

		class ShaderCache : public Foundation::RefObjectCache<ShaderCache, std::string, IShader>
		{
		public:
			IShader* GetShader(ShaderType type, const std::string& name, const IShaderMacros* macros);
			bool AddShader(IShader* shader);
		private:
			static std::string GetKey(ShaderType type, const std::string& name, const IShaderMacros* macros);
		};
	}
}