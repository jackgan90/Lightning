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
			TEXTURE_TYPE type{TEXTURE_TYPE_2D};
			std::uint16_t width{ 0 };
			std::uint16_t height{ 0 };
			union 
			{
				std::uint16_t depth{ 1 };
				std::uint16_t arraySize;
			};
			std::uint16_t numberOfMipmaps{ 1 };
			RenderFormat format{ RenderFormat::UNDEFINED };
			std::uint16_t multiSampleCount{ 1 };
			std::uint16_t multiSampleQuality{ 1 };
		};

		class ITexture : public Plugins::RefCount
		{
		public:
			virtual void Commit() = 0;
		};
	}
}