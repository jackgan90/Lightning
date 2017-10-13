#pragma once
#include <memory>
#include <exception>
#include "renderexception.h"
#include "rendererexportdef.h"
#include "idevice.h"
#include "iswapchain.h"
#include "irendertargetmanager.h"
#include "ipipelinestateobject.h"
#include "color.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API DeviceInitException : public RendererException
		{
		public:
			DeviceInitException(const char*const w):RendererException(w){}
		};

		class LIGHTNINGGE_RENDERER_API SwapChainInitException : public RendererException
		{
		public:
			SwapChainInitException(const char*const w):RendererException(w){}
		};

		class LIGHTNINGGE_RENDERER_API IRenderer
		{
		public:
			virtual ~IRenderer() = default;
			//entry point of render system
			virtual void Render() = 0;
			//return the device of render system
			virtual SharedDevicePtr GetDevice() = 0;
			//return the swap chain of render system
			virtual SharedSwapChainPtr GetSwapChain() = 0;
			//return render target manager
			virtual SharedRenderTargetManagerPtr GetRenderTargetManager() = 0;
			//set default render target clear color.At the beginning of each frame,the back buffer is cleared to this color
			virtual void SetClearColor(const ColorF& color) = 0;
			//get the current frame index
			virtual unsigned long GetCurrentFrameIndex()const = 0;
			//apply the pipeline state object
			virtual void ApplyPipelineStateObject(const PipelineStateObjectPtr& pso) = 0;
		};
		typedef std::unique_ptr<IRenderer> UniqueRendererPtr;
	}
}