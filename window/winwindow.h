#pragma once
#include <string>
#include "window.h"
#include "winwindownativehandle.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		class LIGHTNINGGE_WINDOW_API WinWindow : public Window
		{
		public:
			WinWindow();
			~WinWindow()override;
			bool Init()override;
			bool Show(bool show)override;
			const WindowNativeHandlePtr GetNativeHandle()const override { return m_nativeHandle; }
			WINDOWWIDTH GetWidth()const override;
			WINDOWHEIGHT GetHeight()const override;
			int GetDestroyCode()override;
		private:
			friend LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			friend WinWindow* GetWinWindow(HWND hWnd);
			WindowNativeHandlePtr m_nativeHandle;
			std::string m_Caption;
			WINDOWWIDTH m_width;
			WINDOWHEIGHT m_height;
			int m_destroyCode;
			static char* s_WindowClassName;
		};
	}
}
