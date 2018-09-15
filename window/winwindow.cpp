#include "common.h"
#include "winwindow.h"
#include "logger.h"
#include <vector>

namespace Lightning
{
	namespace WindowSystem
	{
		using Lightning::Foundation::logger;
		using Lightning::Foundation::LogLevel;
		using std::vector;
		char* WinWindow::s_WindowClassName = "DefaultWin32Window";
		
		LRESULT CALLBACK WinWindow::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (uMsg == WM_NCCREATE)
			{
				CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
				LPVOID pThis = pCS->lpCreateParams;
				SetWindowLongPtr(hwnd, 0, reinterpret_cast<LONG_PTR>(pThis));
				auto pWindow = reinterpret_cast<WinWindow*>(pThis);
				pWindow->m_hWnd = hwnd;
				return TRUE;
			}
			auto pWindow = reinterpret_cast<WinWindow*>(GetWindowLongPtrW(hwnd, 0));
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

		WinWindow::WinWindow():m_Caption("Lightning Win32 Window")
			,m_hWnd(nullptr), m_width(800), m_height(600)
		{
			HINSTANCE hInstance = ::GetModuleHandle(NULL);
			RegisterWindowClass(hInstance);
			CreateWindowInternal(hInstance);
			logger.Log(LogLevel::Info, "Create window succeed!");
		}

		void WinWindow::RegisterWindowClass(HINSTANCE hInstance)
		{
			WNDCLASSEX wndcls;
			wndcls.cbSize = sizeof(WNDCLASSEX);
			wndcls.style = CS_HREDRAW | CS_VREDRAW;
			wndcls.lpfnWndProc = WndProc;
			wndcls.cbClsExtra = 0;
			wndcls.cbWndExtra = sizeof(WinWindow*);
			wndcls.hInstance = hInstance;
			wndcls.hIcon = ::LoadIcon(hInstance, IDI_APPLICATION);
			wndcls.hCursor = ::LoadCursor(hInstance, IDC_ARROW);
			wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wndcls.lpszMenuName = NULL;
			wndcls.lpszClassName = s_WindowClassName;
			wndcls.hIconSm = (HICON)::LoadIcon(hInstance, IDI_APPLICATION);

			if (!::RegisterClassEx(&wndcls))
			{
				throw WindowInitException("Register window class failed!ErrorCode : 0x%x", ::GetLastError());
			}
			logger.Log(LogLevel::Info, "Register window class succeed!");
		}

		void WinWindow::CreateWindowInternal(HINSTANCE hInstance)
		{
			RECT windowRect;
			windowRect.left = 0;
			windowRect.right = m_width;
			windowRect.top = 0;
			windowRect.bottom = m_height;
			::AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, NULL);
			logger.Log(LogLevel::Debug, "Window width:%d, window height:%d", windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
			HWND hWnd = ::CreateWindowEx(0, s_WindowClassName, m_Caption.c_str(), 
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
				windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, static_cast<LPVOID>(this));

			if (!hWnd)
			{
				throw WindowInitException("Create window failed!ErrorCode : %x", ::GetLastError());
			}
		}


		WinWindow::~WinWindow()
		{
			if(m_hWnd)
			{ 
				::DestroyWindow(m_hWnd);
				m_hWnd = nullptr;
			}
			logger.Log(LogLevel::Info, "Win32 window destructed!");
		}

		bool WinWindow::Show(bool show)
		{
			if (!m_hWnd)
				return false;
			ShowWindow(m_hWnd, show ? SW_SHOW : SW_HIDE);
			UpdateWindow(m_hWnd);
			MSG msg{};
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

		std::uint32_t WinWindow::GetWidth()const
		{
			return m_width;
		}

		std::uint32_t WinWindow::GetHeight()const
		{
			return m_height;
		}
		int WinWindow::GetDestroyCode()
		{
			return m_destroyCode;
		}
	}
}
