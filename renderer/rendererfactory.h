#pragma once
#include "rendererexportdef.h"
#include "irendercontext.h"
#include "idevice.h"
#include "iswapchain.h"
#include "irendertargetmanager.h"
#include "irenderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API RendererFactory
		{
		public:
			static RenderTargetManagerPtr GetRenderTargetManager(DevicePtr pDevice, SwapChainPtr pSwapChain);
			//TODO some graphics API may coexist more than one context in the application process,try to resovle the condition
			static RenderContextPtr GetRenderContext();
			static RendererPtr GetRenderer(RenderContextPtr pRenderContext);
		private:
			static RenderTargetManagerPtr s_renderTargetMgr;
			static RenderContextPtr s_renderContext;
			static RendererPtr s_renderer;
		};
	}
}