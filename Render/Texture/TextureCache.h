#pragma once
#include "RefObjectCache.h"

namespace Lightning
{
	namespace Render
	{
		class TextureCache : public Foundation::RefObjectCache<ITexture>
		{
		};
	}
}
