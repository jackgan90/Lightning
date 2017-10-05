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
			virtual bool Present() = 0;
			virtual unsigned int GetBufferCount()const = 0;
			virtual RenderTargetPtr GetBufferRenderTarget(unsigned int bufferIndex) = 0;
		};
		using SwapChainPtr = std::shared_ptr<ISwapChain>;
	}
}