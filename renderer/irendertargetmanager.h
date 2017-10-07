#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irenderresourcekeeper.h"
#include "irendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderTargetManager : public IRenderResourceKeeper
		{
		public:
			//create a render target
			virtual RenderTargetPtr CreateRenderTarget() = 0;
			//obtain a render target by ID
			virtual RenderTargetPtr GetRenderTarget(const RenderTargetID&) = 0;
		};
		typedef std::shared_ptr<IRenderTargetManager> RenderTargetManagerPtr;
	}
}