#pragma once
#include "rendererexportdef.h"
#include "idevice.h"
#include "iswapchain.h"
#include "irendertargetmanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		class RendererFactory
		{
		public:
			static RenderTargetManagerPtr GetRenderTargetManager(DevicePtr pDevice, SwapChainPtr pSwapChain);
		};
	}
}