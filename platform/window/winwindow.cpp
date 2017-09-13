#include "winwindow.h"
#include "logging\logger.h"

using LightningGE::Utility::logger;
using LogLevel = LightningGE::Utility::LogLevel;

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
				logger.Log(LogLevel::Info, "Win32 window created!");
				break;
			case WM_DESTROY:
				logger.Log(LogLevel::Info, "WM_DESTROY received!");
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
			logger.Log(LogLevel::Info, "Win32 window constructed!");
		}

		WinWindow::~WinWindow()
		{
			destroy();
			logger.Log(LogLevel::Info, "Win32 window destructed!");
		}

		void WinWindow::destroy()
		{
			if (m_hWnd)
			{
				::DestroyWindow(m_hWnd);
				m_hWnd = nullptr;
			}
		}

		bool WinWindow::Init()
		{
			WNDCLASSEX wndcls;
			HINSTANCE hInstance = ::GetModuleHandle(NULL);
			wndcls.cbSize = sizeof(WNDCLASSEX);
			wndcls.style = CS_HREDRAW | CS_VREDRAW;
			wndcls.lpfnWndProc = WndProc;
			wndcls.cbClsExtra = 0;
			wndcls.cbWndExtra = 0;
			wndcls.hInstance = hInstance;
			wndcls.hIcon = ::LoadIcon(hInstance, IDI_APPLICATION);
			wndcls.hCursor = ::LoadCursor(hInstance, IDC_ARROW);
			wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wndcls.lpszMenuName = NULL;
			wndcls.lpszClassName = s_WindowClassName;
			wndcls.hIconSm = (HICON)::LoadIcon(hInstance, IDI_APPLICATION);

			if (!::RegisterClassEx(&wndcls))
			{
				DWORD error = ::GetLastError();
				logger.Log(LogLevel::Error, "Register window class failed!ErrorCode : 0x%x", error);
				return false;
			}
			logger.Log(LogLevel::Info, "Register window class succeed!");

			m_hWnd = ::CreateWindow(s_WindowClassName, m_Caption.c_str(), 
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
				m_width, m_height, NULL, NULL, hInstance, NULL);

			if (!m_hWnd)
			{
				DWORD error = ::GetLastError();
				logger.Log(LogLevel::Error, "Create window failed!ErrorCode : %x", error);
				return false;
			}
			logger.Log(LogLevel::Info, "Create window succeed!");
			return true;
		}

		bool WinWindow::Show(bool show)
		{
			if (!m_hWnd)
				return false;
			ShowWindow(m_hWnd, show ? SW_SHOW : SW_HIDE);
			UpdateWindow(m_hWnd);
			MSG msg;
			while (::GetMessage(&msg, NULL, 0, 0))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			return true;
		}
	}
}
