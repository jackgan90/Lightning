#pragma once
#include "rendererexportdef.h"
#include "irendertarget.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderTargetManager
		{
		public:
			virtual ~IRenderTargetManager(){}
			virtual RenderTargetPtr CreateRenderTarget() = 0;
			virtual RenderTargetPtr GetRenderTarget(const RenderTargetID&) = 0;
			virtual void ReleaseRenderResources() = 0;
		};
		using RenderTargetManagerPtr = std::shared_ptr<IRenderTargetManager>;
	}
}