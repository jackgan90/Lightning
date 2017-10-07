#include "common.h"
#include "windowmanager.h"
#include "winwindow.h"
#include "logger.h"
#include <vector>

using LightningGE::Foundation::logger;
using LogLevel = LightningGE::Foundation::LogLevel;
using std::vector;

namespace LightningGE
{
	namespace WindowSystem
	{
		char* WinWindow::s_WindowClassName = "DefaultWin32Window";

		WinWindow* GetWinWindow(HWND hWnd)
		{
			auto windows = WindowManager::Instance()->GetAllWindows();
			for (auto window : windows)
			{
				const WindowNativeHandlePtr pHandle = window->GetNativeHandle();
				if (const WinWindowNativeHandle* pWinHandle = dynamic_cast<const WinWindowNativeHandle*>(pHandle.get()))
				{
					if (pWinHandle->OwnWindowsHandle(hWnd))
						return STATIC_CAST_PTR(WinWindow, window);
				}
			}
			return nullptr;
		}

		LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			WinWindow* pWindow = GetWinWindow(hwnd);
			switch (uMsg)
			{
			case WM_SIZE:
			{
				RECT rect;
				::GetClientRect(hwnd, &rect);
				pWindow->m_width = rect.right - rect.left;
				pWindow->m_height = rect.bottom - rect.top;
				//logger.Log(LogLevel::Debug, "window resize.width:%d, height : %d", pWindow->m_width, pWindow->m_height);
				break;
			}
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

		WinWindow::WinWindow():m_nativeHandle(nullptr), m_Caption("Lightning Win32 Window")
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
			if (m_nativeHandle)
			{
				::DestroyWindow(STATIC_CAST_PTR(WinWindowNativeHandle, m_nativeHandle)->m_hWnd);
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

			RECT windowRect;
			windowRect.left = 0;
			windowRect.right = m_width;
			windowRect.top = 0;
			windowRect.bottom = m_height;
			::AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, NULL);
			logger.Log(LogLevel::Debug, "Window width:%d, window height:%d", windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
			HWND hWnd = ::CreateWindow(s_WindowClassName, m_Caption.c_str(), 
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
				windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, NULL);

			if (!hWnd)
			{
				DWORD error = ::GetLastError();
				logger.Log(LogLevel::Error, "Create window failed!ErrorCode : %x", error);
				return false;
			}
			m_nativeHandle = std::make_shared<WinWindowNativeHandle>(hWnd);
			logger.Log(LogLevel::Info, "Create window succeed!");
			return true;
		}

		bool WinWindow::Show(bool show)
		{
			auto hWnd = STATIC_CAST_PTR(WinWindowNativeHandle, m_nativeHandle)->m_hWnd;
			if (!m_nativeHandle || !hWnd)
				return false;
			ShowWindow(hWnd, show ? SW_SHOW : SW_HIDE);
			UpdateWindow(hWnd);
			MSG msg;
			while (true)
			{
				if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
					{
						m_destroyCode = msg.wParam;	
						break;
					}
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				else
				{
					Window::OnIdle();
				}
			}
			return true;
		}

		WINDOWWIDTH WinWindow::GetWidth()const
		{
			return m_width;
		}

		WINDOWHEIGHT WinWindow::GetHeight()const
		{
			return m_height;
		}
		int WinWindow::GetDestroyCode()
		{
			return m_destroyCode;
		}
	}
}
