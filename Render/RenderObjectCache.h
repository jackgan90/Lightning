#pragma once
#include "RefObjectCache.h"
#include "Texture/ITexture.h"
#include "IShader.h"

namespace Lightning
{
	namespace Render
	{
		using TextureCache = Foundation::RefObjectCache<ITexture>;
		using ShaderCache = Foundation::RefObjectCache<IShader>;
	}
}