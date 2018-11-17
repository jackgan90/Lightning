#pragma once
#include "Common.h"
#include "Singleton.h"
#include "IRenderer.h"
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
