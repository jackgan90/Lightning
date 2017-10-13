#pragma once
#include "common.h"
#include "singleton.h"
#include "rendererexportdef.h"
#include "irenderer.h"
#include "iwindow.h"
#include "filesystem.h"


namespace LightningGE
{
	namespace Renderer
	{
		using WindowSystem::WindowPtr;
		using Foundation::SharedFileSystemPtr;
		class LIGHTNINGGE_RENDERER_API RendererFactory : public Foundation::Singleton<RendererFactory>
		{
		public:
			UniqueRendererPtr CreateRenderer(const WindowPtr& pWindow, const SharedFileSystemPtr& fs)const;
		};
	}
}