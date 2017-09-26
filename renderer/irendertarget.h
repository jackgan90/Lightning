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
		};
		using RenderTargetPtr = std::shared_ptr<IRenderTarget>;
	}
}