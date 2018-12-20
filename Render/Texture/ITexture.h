#pragma once
#include <memory>
#include <cstdint>
#include "RenderConstants.h"
#include "RefObject.h"

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
			void Reset()
			{
				type = TEXTURE_TYPE_2D;
				width = height = 0;
				depth = 0;
				numberOfMipmaps = 1;
				format = RenderFormat::UNDEFINED;
				multiSampleCount = 1;
				multiSampleQuality = 1;
			}
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
		static_assert(std::is_pod<TextureDescriptor>::value, "TextureDescriptor is not a POD type.");

		class ITexture : public Plugins::RefObject
		{
		public:
			virtual void Commit() = 0;
		};
	}
}