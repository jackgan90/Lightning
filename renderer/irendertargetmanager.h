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
			virtual RenderTargetPtr CreateRenderTarget() = 0;
			virtual RenderTargetPtr GetRenderTarget(const RenderTargetID&) = 0;
		};
		using RenderTargetManagerPtr = std::shared_ptr<IRenderTargetManager>;
	}
}