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
			virtual std::uint32_t INTERFACECALL GetSampleCount()const = 0;
			virtual int INTERFACECALL GetSampleQuality()const = 0;
			virtual RenderFormat INTERFACECALL GetRenderFormat()const = 0;
			virtual std::uint32_t INTERFACECALL GetCurrentBackBufferIndex()const = 0;
			//get corresponding render target
			virtual IRenderTarget* INTERFACECALL GetDefaultRenderTarget() = 0;
		};
	}
}
