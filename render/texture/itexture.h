#pragma once
#include <memory>
#include "rendererexportdef.h"

namespace Lightning
{
	namespace Render
	{
		enum TEXTURE_TYPE
		{
			TEXTURE_TYPE_2D = 0,
			TEXTURE_TYPE_3D,
			TEXTURE_TYPE_2D_ARRAY,
			TEXTURE_TYPE_NUM
		};

		struct LIGHTNING_RENDER_API TextureDescriptor
		{
			TEXTURE_TYPE type;
		};

		class LIGHTNING_RENDER_API ITexture
		{
		public:
			virtual ~ITexture(){}
			virtual void Commit() = 0;
		};
		using SharedTexturePtr = std::shared_ptr<ITexture>;
	}
}