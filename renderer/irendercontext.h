#pragma once
#include "rendererexportdef.h"
#include "iwindow.h"
#include "idevice.h"
#include "iswapchain.h"

namespace LightningGE
{
	namespace Renderer
	{
		//interface for initialize rendering context
		using WindowSystem::WindowPtr;
		class LIGHTNINGGE_RENDERER_API IRenderContext
		{
		public:
			virtual ~IRenderContext(){}
			virtual bool Init(WindowPtr pWindow) = 0;
			virtual DevicePtr GetDevice() = 0;
			virtual SwapChainPtr GetSwapChain() = 0;
			virtual void Render() = 0;
			virtual void ReleaseRenderResources() = 0;
		};
		using RenderContextPtr = std::shared_ptr<IRenderContext>;
	}
}
