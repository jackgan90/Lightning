#pragma once
#include <string>
#include <Windows.h>
#include "GameWindow.h"

namespace Lightning
{
	namespace Window
	{
		class WindowsGameWindow : public GameWindow
		{
		public:
			WindowsGameWindow();
			INTERFACECALL ~WindowsGameWindow()override;
			bool INTERFACECALL Show(bool show)override;
			void INTERFACECALL Update()override;
			HWND GetWindowHandle()const { return mHwnd; }
		private:
			static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			void RegisterWindowClass(HINSTANCE hInstance);
			void CreateWindowInternal(HINSTANCE	hInstance);
			static constexpr char* WindowsGameWindow::sWindowClassName = "DefaultWin32Window";
			HWND mHwnd;
		};
	}
}
