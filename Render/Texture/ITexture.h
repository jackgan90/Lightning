#pragma once
#include <memory>
#include <cstdint>
#include "RenderConstants.h"
#include "RefCount.h"

namespace Lightning
{
	namespace Render
	{
		enum TEXTURE_TYPE
		{
			TEXTURE_TYPE_1D = 0,
			TEXTURE_TYPE_1D_ARRAY,
			TEXTURE_TYPE_2D,
			TEXTURE_TYPE_2D_ARRAY,
			TEXTURE_TYPE_3D,
			TEXTURE_TYPE_NUM
		};

		struct TextureDescriptor
		{
			TEXTURE_TYPE type;
			std::uint16_t width;
			std::uint16_t height;
			union 
			{
				std::uint16_t depth;
				std::uint16_t arraySize;
			};
			std::uint16_t numberOfMipmaps;
			RenderFormat format;
			std::uint16_t multiSampleCount;
			std::uint16_t multiSampleQuality;
		};

		class ITexture : public Plugins::RefCount
		{
		public:
			virtual void Commit() = 0;
		};
	}
}