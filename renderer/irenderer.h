#pragma once
#include "rendererexportdef.h"
#include "irenderresourceuser.h"
#include "idevice.h"
#include "iswapchain.h"
#include "irendercontext.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderer : public IRenderResourceUser
		{
		public:
			virtual void Render() = 0;
			virtual DevicePtr GetDevice() = 0;
			virtual SwapChainPtr GetSwapChain() = 0;
			virtual RenderContextPtr CreateRenderContext() = 0;
		};
		using RendererPtr = std::shared_ptr<IRenderer>;
	}
}