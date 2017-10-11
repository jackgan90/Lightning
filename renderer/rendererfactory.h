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
		using Foundation::FileSystemPtr;
		class LIGHTNINGGE_RENDERER_API RendererFactory : public Foundation::Singleton<RendererFactory>
		{
		public:
			IRenderer* CreateRenderer(const WindowPtr& pWindow, const FileSystemPtr& fs)const;
		};
	}
}