#pragma once
#include <memory>
#include <vector>
#include "rendererexportdef.h"
#include "renderconstants.h"

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
			//get pixel sample count
			virtual std::size_t GetSampleCount() = 0;
			//get pixel sample quality
			virtual int GetSampleQuality() = 0;
			//get render format
			virtual RenderFormat GetRenderFormat()const = 0;
			//return the attached RT ID
			virtual RenderTargetID GetID()const = 0;
			virtual ~IRenderTarget() = default;
		};
		using SharedRenderTargetPtr = std::shared_ptr<IRenderTarget>;
		using RenderTargetList = std::vector<IRenderTarget*>;
	}
}