#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irenderresourcekeeper.h"

namespace LightningGE
{
	namespace Renderer
	{
		typedef int RenderTargetID;
		class LIGHTNINGGE_RENDERER_API IRenderTarget : public IRenderResourceKeeper
		{
		public:
			//return if the render target representing a swap chain buffer(a back buffer)
			virtual bool IsSwapChainRenderTarget()const = 0;
			//return the attached RT ID
			virtual RenderTargetID GetID()const = 0;
		};
		typedef std::shared_ptr<IRenderTarget> RenderTargetPtr;
	}
}