#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irenderresourcekeeper.h"
#include "idevice.h"
#include "iswapchain.h"
#include "irendercontext.h"
#include "ipipelinestateobject.h"
#include "color.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderer : public IRenderResourceKeeper
		{
		public:
			//entry point of render system
			virtual void Render() = 0;
			//return the device of render system
			virtual DevicePtr GetDevice() = 0;
			//return the swap chain of render system
			virtual SwapChainPtr GetSwapChain() = 0;
			//create a platform dependent render context
			virtual RenderContextPtr CreateRenderContext() = 0;
			//set default render target clear color.At the begining of each frame,the back buffer is cleared to this color
			virtual void SetClearColor(const ColorF& color) = 0;
			//get the current frame index
			virtual unsigned long GetCurrentFrameIndex()const = 0;
			//apply the pipeline state object
			virtual void ApplyPipelineStateObject(const PipelineStateObjectPtr& pso) = 0;
		};
		typedef std::shared_ptr<IRenderer> RendererPtr;
	}
}