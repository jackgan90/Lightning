#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irenderresourceuser.h"

namespace LightningGE
{
	namespace Renderer
	{
		using RenderTargetID = int;
		class LIGHTNINGGE_RENDERER_API IRenderTarget : public IRenderResourceUser
		{
		public:
			virtual bool IsSwapChainRenderTarget()const = 0;
			virtual RenderTargetID GetID()const = 0;
		};
		using RenderTargetPtr = std::shared_ptr<IRenderTarget>;
	}
}