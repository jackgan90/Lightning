#pragma once
#include <memory>
#include <vector>
#include "rendererexportdef.h"

namespace LightningGE
{
	namespace Render
	{
		using RenderTargetID = int;
		class LIGHTNINGGE_RENDER_API IRenderTarget
		{
		public:
			//return if the render target representing a swap chain buffer(a back buffer)
			virtual bool IsSwapChainRenderTarget()const = 0;
			//returns the bound index of GPU.typically a GPU is able to bind multiple render targets at the same time.
			//Each bound render target has a bound index.
			virtual int GetBoundIndex()const = 0;
			//return the attached RT ID
			virtual RenderTargetID GetID()const = 0;
			virtual ~IRenderTarget() = default;
		};
		using SharedRenderTargetPtr = std::shared_ptr<IRenderTarget>;
		using RenderTargetList = std::vector<IRenderTarget*>;
	}
}