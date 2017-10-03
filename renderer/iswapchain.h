#pragma once
#include "irenderresourceuser.h"
#include "irendertarget.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API ISwapChain : public IRenderResourceUser
		{
		public:
			virtual bool Present() = 0;
			virtual unsigned int GetBufferCount()const = 0;
			virtual RenderTargetPtr GetBufferRenderTarget(unsigned int bufferIndex) = 0;
		};
		using SwapChainPtr = std::shared_ptr<ISwapChain>;
	}
}