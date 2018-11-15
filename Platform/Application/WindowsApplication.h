#pragma once
#include "PlatformExportDef.h"
#include "Application.h"
#include "GameWindow.h"
#include "FileSystem.h"

namespace Lightning
{
	namespace App
	{
		using Window::SharedWindowPtr;
		class WindowsApplication : public Application
		{
		public:
			WindowsApplication();
			~WindowsApplication()override;
			void Update()override;
		protected:
			void OnMouseWheel(const Window::MouseWheelEvent& event);
			void OnKeyDown(const Window::KeyEvent& event);
			void OnMouseDown(const Window::MouseDownEvent& event);
			void OnMouseMove(const Window::MouseMoveEvent& event);
			void RegisterWindowHandlers()override;
			SharedWindowPtr CreateMainWindow()override;
			UniqueRendererPtr CreateRenderer()override;
		};
	}
}
