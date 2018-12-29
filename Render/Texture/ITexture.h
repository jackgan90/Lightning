#pragma once
#include <memory>
#include <cstdint>
#include "RenderConstants.h"
#include "IRefObject.h"

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
			virtual void INTERFACECALL Commit() = 0;
			virtual std::uint16_t INTERFACECALL GetMultiSampleCount()const = 0;
			virtual std::uint16_t INTERFACECALL GetMultiSampleQuality()const = 0;
			virtual RenderFormat INTERFACECALL GetRenderFormat()const = 0;
			virtual std::size_t INTERFACECALL GetWidth()const = 0;
			virtual std::size_t INTERFACECALL GetHeight()const = 0;
		};
	}
}