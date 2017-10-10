#pragma once
#include <memory>
#include "counter.h"
#include "rendererexportdef.h"
#include "irendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderTargetManager : public Foundation::Counter<IRenderTargetManager, 1>
		{
		public:
			//create a render target
			virtual RenderTargetPtr CreateRenderTarget() = 0;
			//obtain a render target by ID
			virtual RenderTargetPtr GetRenderTarget(const RenderTargetID&) = 0;
		};
	}
}