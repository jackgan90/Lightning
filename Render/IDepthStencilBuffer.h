#pragma once
#include <cstdint>
#include "RenderConstants.h"
#include "Texture/ITexture.h"
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		struct IDepthStencilBuffer : Plugins::IRefObject
		{
			virtual void INTERFACECALL SetClearValue(float depthValue=1.0f, std::uint32_t stencilValue=0) = 0;
			virtual float INTERFACECALL GetDepthClearValue()const = 0;
			virtual std::uint8_t INTERFACECALL GetStencilClearValue()const = 0;
			virtual ITexture* INTERFACECALL GetTexture()const = 0;
		};
	}
}
