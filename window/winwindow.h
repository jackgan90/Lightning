#pragma once
#include <string>
#include <Windows.h>
#include "window.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		class LIGHTNINGGE_WINDOW_API WinWindow : public Window
		{
		public:
			WinWindow();
			~WinWindow()override;
			bool Show(bool show)override;
			WINDOWWIDTH GetWidth()const override;
			WINDOWHEIGHT GetHeight()const override;
			int GetDestroyCode()override;
			HWND GetWindowHandle()const { return m_hWnd; }
		private:
			static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			void RegisterWindowClass(HINSTANCE hInstance);
			void CreateWindowInternal(HINSTANCE	hInstance);
			std::string m_Caption;
			WINDOWWIDTH m_width;
			WINDOWHEIGHT m_height;
			HWND m_hWnd;
			int m_destroyCode;
			static char* s_WindowClassName;
		};
	}
}
