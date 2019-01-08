#pragma once
#include <memory>
#include <cstdint>
#include "RenderConstants.h"
#include "IRefObject.h"

namespace Lightning
{
	namespace Render
	{
		enum TextureDimension
		{
			TEXTURE_DIMENSION_1D = 0,
			TEXTURE_DIMENSION_1D_ARRAY,
			TEXTURE_DIMENSION_2D,
			TEXTURE_DIMENSION_2D_ARRAY,
			TEXTURE_DIMENSION_3D,
			TEXTURE_DIMENSION_NUM
		};

		struct TextureDescriptor
		{
			void Reset()
			{
				dimension = TEXTURE_DIMENSION_2D;
				width = height = 0;
				depth = 0;
				numberOfMipmaps = 1;
				format = RenderFormat::UNDEFINED;
				multiSampleCount = 1;
				multiSampleQuality = 1;
			}
			TextureDimension dimension;
			std::size_t width;
			std::size_t height;
			union 
			{
				std::size_t depth;
				std::size_t arraySize;
			};
			std::size_t numberOfMipmaps;
			RenderFormat format;
			std::size_t multiSampleCount;
			std::size_t multiSampleQuality;
			float depthClearValue;
			std::uint8_t stencilClearValue;
		};
		static_assert(std::is_pod<TextureDescriptor>::value, "TextureDescriptor is not a POD type.");

		struct ITexture : Plugins::IRefObject
		{
			virtual TextureDimension INTERFACECALL GetDimension()const = 0;
			virtual void INTERFACECALL Commit() = 0;
			virtual std::uint16_t INTERFACECALL GetMultiSampleCount()const = 0;
			virtual std::uint16_t INTERFACECALL GetMultiSampleQuality()const = 0;
			virtual RenderFormat INTERFACECALL GetRenderFormat()const = 0;
			virtual std::size_t INTERFACECALL GetWidth()const = 0;
			virtual std::size_t INTERFACECALL GetHeight()const = 0;
			//If this texture is a 3D texture, returns the depth,otherwise returns array size
			virtual std::size_t INTERFACECALL GetDepth()const = 0;
		};
	}
}