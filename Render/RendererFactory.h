#pragma once
#include "IRenderer.h"
#include "Singleton.h"
#include "IWindow.h"


namespace Lightning
{
	namespace Render
	{
		class RendererFactory : public Foundation::Singleton<RendererFactory>
		{
		public:
			IRenderer* CreateRenderer(Window::IWindow* window)const;
		};
	}
}
