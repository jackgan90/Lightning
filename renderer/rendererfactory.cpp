#include "rendererfactory.h"
#ifdef LIGHTNINGGE_USE_D3D12
#include "d3d12rendertargetmanager.h"
#include "d3d12rendercontext.h"
#include "d3d12renderer.h"
#endif


namespace LightningGE
{
	namespace Renderer
	{
		RenderTargetManagerPtr RendererFactory::s_renderTargetMgr;
		RenderContextPtr RendererFactory::s_renderContext;
		RendererPtr RendererFactory::s_renderer;
		RenderTargetManagerPtr RendererFactory::CreateRenderTargetManager(DevicePtr pDevice, SwapChainPtr pSwapChain)
		{
			if (!s_renderTargetMgr)
			{
#ifdef LIGHTNINGGE_USE_D3D12
				s_renderTargetMgr = RenderTargetManagerPtr(new D3D12RenderTargetManager(pDevice, pSwapChain));
#endif // LIGHTNINGGE_USE_D3D12
			}
			return s_renderTargetMgr;
		}

		RenderContextPtr RendererFactory::CreateRenderContext()
		{
			if (!s_renderContext)
			{
#ifdef LIGHTNINGGE_USE_D3D12
				s_renderContext = RenderContextPtr(new D3D12RenderContext());
#endif // LIGHTNINGGE_USE_D3D12
			}
			return s_renderContext;
		}

		RendererPtr RendererFactory::CreateRenderer(RenderContextPtr pRenderContext)
		{
			if (!s_renderer)
			{
#ifdef LIGHTNINGGE_USE_D3D12
				s_renderer = RendererPtr(new D3D12Renderer(pRenderContext));
#endif // LIGHTNINGGE_USE_D3D12
				
			}
			return s_renderer;
		}

	}
}