#pragma once
#include "IRenderTarget.h"
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		struct ISwapChain
		{
			virtual INTERFACECALL ~ISwapChain() = default;
			//preset the current back buffer to display
			virtual bool INTERFACECALL Present() = 0;
			virtual void INTERFACECALL Resize(std::uint32_t width, std::uint32_t height) = 0;
			virtual std::size_t INTERFACECALL GetMultiSampleCount()const = 0;
			virtual std::size_t INTERFACECALL GetMultiSampleQuality()const = 0;
			virtual RenderFormat INTERFACECALL GetRenderFormat()const = 0;
			//get corresponding render target
			virtual IRenderTarget* INTERFACECALL GetCurrentRenderTarget() = 0;
		};
	}
}
