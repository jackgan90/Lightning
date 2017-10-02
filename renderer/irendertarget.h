#pragma once
#include "rendererexportdef.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderTarget
		{
		public:
			virtual ~IRenderTarget() {}
			virtual void ReleaseRenderResources() = 0;
		};
		using RenderTargetPtr = std::shared_ptr<IRenderTarget>;
		using RenderTargetID = int;
	}
}