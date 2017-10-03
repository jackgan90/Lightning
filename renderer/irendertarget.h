#pragma once
#include "rendererexportdef.h"
#include "irenderresourceuser.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderTarget : public IRenderResourceUser
		{
		};
		using RenderTargetPtr = std::shared_ptr<IRenderTarget>;
		using RenderTargetID = int;
	}
}