#pragma once
#include <string>
#include <Windows.h>
#include "window.h"

namespace Lightning
{
	namespace WindowSystem
	{
		class LIGHTNING_WINDOW_API WinWindow : public Window
		{
		public:
			WinWindow();
			~WinWindow()override;
			bool Show(bool show)override;
			std::uint32_t GetWidth()const override;
			std::uint32_t GetHeight()const override;
			int GetDestroyCode()override;
			HWND GetWindowHandle()const { return mHwnd; }
		private:
			static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			void RegisterWindowClass(HINSTANCE hInstance);
			void CreateWindowInternal(HINSTANCE	hInstance);
			std::string mCaption;
			std::uint32_t mWidth;
			std::uint32_t mHeight;
			HWND mHwnd;
			int mExitCode;
			static char* sWindowClassName;
		};
	}
}
