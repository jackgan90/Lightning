#pragma once
#include <cstdint>
#include "RenderConstants.h"
#include "IRefObject.h"
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		struct IDepthStencilBuffer : Plugins::IRefObject
		{
			virtual void INTERFACECALL SetClearValue(float depthValue=1.0f, std::uint32_t stencilValue=0) = 0;
			virtual float INTERFACECALL GetDepthClearValue()const = 0;
			virtual std::uint32_t INTERFACECALL GetStencilClearValue()const = 0;
			virtual RenderFormat INTERFACECALL GetRenderFormat()const = 0;
			virtual std::uint32_t INTERFACECALL GetWidth()const = 0;
			virtual std::uint32_t INTERFACECALL GetHeight()const = 0;
		};
	}
}
