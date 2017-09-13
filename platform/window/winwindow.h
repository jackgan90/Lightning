#pragma once
#include "iwindow.h"
#include <Windows.h>
#include <string>

namespace LightningGE
{
	namespace WindowSystem
	{
		class LIGHTNINGGE_PLATFORM_API WinWindow : public IWindow
		{
		public:
			WinWindow();
			void destroy()override;
			~WinWindow()override;
			bool Init()override;
			bool Show(bool show)override;
		private:
			HWND m_hWnd;
			std::string m_Caption;
			unsigned int m_width;
			unsigned int m_height;
			static char* s_WindowClassName;
		};
	}
}
