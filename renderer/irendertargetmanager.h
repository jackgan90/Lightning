#pragma once
#include "rendererexportdef.h"
#include "irenderresourceuser.h"
#include "irendertarget.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderTargetManager : public IRenderResourceUser
		{
		public:
			virtual RenderTargetPtr CreateRenderTarget() = 0;
			virtual RenderTargetPtr GetRenderTarget(const RenderTargetID&) = 0;
		};
		using RenderTargetManagerPtr = std::shared_ptr<IRenderTargetManager>;
	}
}