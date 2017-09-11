#include "winwindow.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		char* WinWindow::s_WindowClassName = "DefaultWin32Window";

		LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch (uMsg)
			{
			case WM_CREATE:
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
			return 0;
		}

		WinWindow::WinWindow():m_hWnd(nullptr), m_Caption("Lightning Win32 Window")
			,m_width(800), m_height(600)
		{

		}

		WinWindow::~WinWindow()
		{

		}

		void WinWindow::destroy()
		{

		}

		bool WinWindow::Init()
		{
			WNDCLASSEX wndcls;
			HINSTANCE hInstance = GetModuleHandle(NULL);
			wndcls.cbSize = sizeof(WNDCLASSEX);
			wndcls.style = CS_HREDRAW | CS_VREDRAW;
			wndcls.lpfnWndProc = WndProc;
			wndcls.cbClsExtra = 0;
			wndcls.cbWndExtra = 0;
			wndcls.hInstance = hInstance;
			wndcls.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
			wndcls.hCursor = LoadCursor(hInstance, IDC_ARROW);
			wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wndcls.lpszMenuName = NULL;
			wndcls.lpszClassName = s_WindowClassName;
			wndcls.hIconSm = (HICON)LoadIcon(hInstance, IDI_APPLICATION);

			if (!RegisterClassEx(&wndcls))
			{
				//TODO : add log
				return false;
			}

			m_hWnd = CreateWindow(s_WindowClassName, m_Caption.c_str(), 
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
				m_width, m_height, NULL, NULL, hInstance, NULL);

			if (!m_hWnd)
			{
				//TODO : add log
				return false;
			}
			return true;
		}

		bool WinWindow::Show(bool show)
		{
			if (!m_hWnd)
				return false;
			ShowWindow(m_hWnd, show ? SW_SHOW : SW_HIDE);
			UpdateWindow(m_hWnd);
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return true;
		}
	}
}
