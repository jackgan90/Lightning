#pragma once
#include "rendererexportdef.h"
#include "irenderresourceuser.h"
#include "iwindow.h"
#include "idevice.h"
#include "iswapchain.h"

namespace LightningGE
{
	namespace Renderer
	{
		//interface for initialize rendering context
		using WindowSystem::WindowPtr;
		class LIGHTNINGGE_RENDERER_API IRenderContext : public IRenderResourceUser
		{
		public:
			virtual bool Init(WindowPtr pWindow) = 0;
		};
		using RenderContextPtr = std::shared_ptr<IRenderContext>;
	}
}
