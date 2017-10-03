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
			static RenderTargetManagerPtr CreateRenderTargetManager(DevicePtr pDevice, SwapChainPtr pSwapChain);
			//TODO some graphics API may coexist more than one context in the application process,try to resovle the condition
			static RenderContextPtr CreateRenderContext();
			static RendererPtr CreateRenderer(RenderContextPtr pRenderContext);
			static RenderTargetManagerPtr GetRenderTargetManager() { return s_renderTargetMgr; }
			static RenderContextPtr GetRenderContext() { return s_renderContext; }
			static RendererPtr GetRenderer() { return s_renderer; }
		private:
			static RenderTargetManagerPtr s_renderTargetMgr;
			static RenderContextPtr s_renderContext;
			static RendererPtr s_renderer;
		};
	}
}