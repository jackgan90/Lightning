#pragma once
#include <memory>
#include <exception>
#include "irendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API ISwapChain
		{
		public:
			virtual ~ISwapChain() = default;
			//preset the current back buffer to display
			virtual bool Present() = 0;
			//get swapchain back buffer count
			virtual unsigned int GetBufferCount()const = 0;
			//get corresponding render target
			virtual RenderTargetPtr GetBufferRenderTarget(unsigned int bufferIndex) = 0;
		};
		typedef std::shared_ptr<ISwapChain> SharedSwapChainPtr;
	}
}