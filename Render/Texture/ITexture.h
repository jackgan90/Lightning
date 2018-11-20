#pragma once
#include <memory>
#include "RefCount.h"

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

		struct TextureDescriptor
		{
			TEXTURE_TYPE type;
		};

		class ITexture : Plugins::RefCount
		{
		public:
			virtual ~ITexture(){}
			virtual void Commit() = 0;
		};
	}
}