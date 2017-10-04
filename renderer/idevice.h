#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "irenderresourceuser.h"
#include "rect.h"
#include "color.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IDevice : public IRenderResourceUser
		{
		public:
			virtual void ClearRenderTarget(const RenderTargetPtr& rt, const ColorF& color, const RectI* pRects = nullptr, const int rectCount=0) = 0;
		};
		using DevicePtr = std::shared_ptr<IDevice>;
	}
}
