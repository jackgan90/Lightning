#pragma once
#include "rendererexportdef.h"
#include "irenderresourcekeeper.h"
#include "iwindow.h"
#include "idevice.h"
#include "iswapchain.h"

namespace LightningGE
{
	namespace Renderer
	{
		//interface for initialize rendering context
		using WindowSystem::WindowPtr;
		class LIGHTNINGGE_RENDERER_API IRenderContext : public IRenderResourceKeeper
		{
		public:
			virtual bool Init(const WindowPtr& pWindow) = 0;
		};
		using RenderContextPtr = std::shared_ptr<IRenderContext>;
	}
}
