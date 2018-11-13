#pragma once
#include "Common.h"
#include "Singleton.h"
#include "RendererExportDef.h"
#include "IRenderer.h"
#include "WindowSystem.h"
#include "FileSystem.h"


namespace Lightning
{
	namespace Render
	{
		using Window::SharedWindowPtr;
		using Foundation::SharedFileSystemPtr;
		using UniqueRendererPtr = std::unique_ptr<IRenderer>;
		class LIGHTNING_RENDER_API RendererFactory : public Foundation::Singleton<RendererFactory>
		{
		public:
			UniqueRendererPtr CreateRenderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs)const;
		};
	}
}
