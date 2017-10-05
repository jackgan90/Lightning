#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irenderresourcekeeper.h"
#include "idevice.h"
#include "iswapchain.h"
#include "irendercontext.h"
#include "color.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderer : public IRenderResourceKeeper
		{
		public:
			virtual void Render() = 0;
			virtual DevicePtr GetDevice() = 0;
			virtual SwapChainPtr GetSwapChain() = 0;
			virtual RenderContextPtr CreateRenderContext() = 0;
			virtual void SetClearColor(const ColorF& color) = 0;
			virtual unsigned long GetCurrentFrameIndex()const = 0;
		};
		using RendererPtr = std::shared_ptr<IRenderer>;
	}
}