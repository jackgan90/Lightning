#include "rendererfactory.h"
#ifdef LIGHTNINGGE_WIN32
#include "d3d12/d3d12rendertargetmanager.h"
#endif


namespace LightningGE
{
	namespace Renderer
	{
		RenderTargetManagerPtr RendererFactory::GetRenderTargetManager(DevicePtr pDevice, SwapChainPtr pSwapChain)
		{
#ifdef LIGHTNINGGE_WIN32
			return RenderTargetManagerPtr(new D3D12RenderTargetManager(pDevice, pSwapChain));
#endif // LIGHTNINGGE_WIN32

			return RenderTargetManagerPtr();
		}

	}
}