#pragma once
#include <memory>
#include <exception>
#include "irendertarget.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API ISwapChain
		{
		public:
			virtual ~ISwapChain() = default;
			//preset the current back buffer to display
			virtual bool Present() = 0;
			virtual std::size_t GetSampleCount()const = 0;
			virtual int GetSampleQuality()const = 0;
			virtual RenderFormat GetRenderFormat()const = 0;
			virtual std::size_t GetCurrentBackBufferIndex()const = 0;
			//get corresponding render target
			virtual SharedRenderTargetPtr GetBufferRenderTarget(unsigned int bufferIndex) = 0;
			virtual SharedRenderTargetPtr GetPrimaryRenderTarget() = 0;
		};
		using SharedSwapChainPtr = std::shared_ptr<ISwapChain>;
	}
}