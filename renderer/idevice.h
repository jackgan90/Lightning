#pragma once
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "irenderresourceuser.h"
#include "color.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IDevice : public IRenderResourceUser
		{
		public:
			virtual void ClearRenderTarget(const RenderTargetPtr& rt, const Color& color) = 0;
		};
		using DevicePtr = std::shared_ptr<IDevice>;
	}
}
