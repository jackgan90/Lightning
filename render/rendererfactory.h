#pragma once
#include "common.h"
#include "singleton.h"
#include "rendererexportdef.h"
#include "irenderer.h"
#include "iwindow.h"
#include "filesystem.h"


namespace LightningGE
{
	namespace Render
	{
		using WindowSystem::SharedWindowPtr;
		using Foundation::SharedFileSystemPtr;
		using UniqueRendererPtr = std::unique_ptr<IRenderer>;
		class LIGHTNINGGE_RENDER_API RendererFactory : public Foundation::Singleton<RendererFactory>
		{
		public:
			UniqueRendererPtr CreateRenderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs)const;
		};
	}
}