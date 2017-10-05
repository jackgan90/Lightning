#pragma once
#include <memory>
#include "irenderresourcekeeper.h"
#include "irendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API ISwapChain : public IRenderResourceKeeper
		{
		public:
			//preset the current back buffer to display
			virtual bool Present() = 0;
			//get swapchain back buffer count
			virtual unsigned int GetBufferCount()const = 0;
			//get corresponding render target
			virtual RenderTargetPtr GetBufferRenderTarget(unsigned int bufferIndex) = 0;
		};
		using SwapChainPtr = std::shared_ptr<ISwapChain>;
	}
}