#include "common.h"
#include "winwindow.h"
#include "logger.h"
#include "container.h"

namespace Lightning
{
	namespace WindowSystem
	{
		char* WinWindow::sWindowClassName = "DefaultWin32Window";
		
		LRESULT CALLBACK WinWindow::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (uMsg == WM_NCCREATE)
			{
				CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
				LPVOID pThis = pCS->lpCreateParams;
				SetWindowLongPtr(hwnd, 0, reinterpret_cast<LONG_PTR>(pThis));
				auto pWindow = reinterpret_cast<WinWindow*>(pThis);
				pWindow->mHwnd = hwnd;
				return TRUE;
			}
			auto pWindow = reinterpret_cast<WinWindow*>(GetWindowLongPtrW(hwnd, 0));
			switch (uMsg)
			{
			case WM_SIZE:
			{
				RECT rect;
				::GetClientRect(hwnd, &rect);
				pWindow->mWidth = rect.right - rect.left;
				pWindow->mHeight = rect.bottom - rect.top;
				WindowResizeParam param(pWindow);
				param.width = pWindow->mWidth;
				param.height = pWindow->mHeight;
				pWindow->PostWindowMessage(WindowMessage::RESIZE, param);
				//logger.Log(LogLevel::Debug, "window resize.width:%d, height : %d", pWindow->mWidth, pWindow->mHeight);
				break;
			}
			case WM_MOUSEWHEEL:
			{
				MouseWheelParam param(pWindow);
				param.is_vertical = true;
				param.wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
				pWindow->PostWindowMessage(WindowMessage::MOUSE_WHEEL, param);
				break;
			}
			case WM_KEYDOWN:
			{
				KeyParam param(pWindow);
				std::size_t code{ 0 };
				if (wParam >= 'a' && wParam <= 'z')
					code = wParam - 'a' + VK_A;
				if(wParam >= 'A' && wParam <= 'Z')
					code = wParam - 'A' + VK_A;
				auto CtrlState = ::GetKeyState(VK_CONTROL);
				auto LButtonState = ::GetKeyState(VK_LBUTTON);
				auto RButtonState = ::GetKeyState(VK_RBUTTON);
				auto MButtonState = ::GetKeyState(VK_MBUTTON);
				auto ShiftState = ::GetKeyState(VK_SHIFT);
				std::size_t StateMask = 0x8000;
				if (CtrlState & StateMask)
				{
					code |= VK_CTRL;
				}
				if (LButtonState & StateMask)
				{
					code |= VK_MOUSELBUTTON;
				}
				if (MButtonState & StateMask)
				{
					code |= VK_MOUSEMBUTTON;
				}
				if (RButtonState & StateMask)
				{
					code |= VK_MOUSERBUTTON;
				}
				if (ShiftState & StateMask)
				{
					code |= VK_SHIFTBUTTON;
				}
				param.code = static_cast<VirtualKeyCode>(code);
				pWindow->PostWindowMessage(WindowMessage::KEY_DOWN, param);
				break;
			}
			case WM_RBUTTONDOWN:
			{
				MouseDownParam param(pWindow);
				param.x = LOWORD(lParam);
				param.y = HIWORD(lParam);
				std::size_t pressedKey = VK_MOUSERBUTTON;
				if (wParam & MK_CONTROL)
					pressedKey |= VK_CTRL;
				if (wParam & MK_LBUTTON)
					pressedKey |= VK_MOUSELBUTTON;
				if (wParam & MK_MBUTTON)
					pressedKey |= VK_MOUSEMBUTTON;
				if (wParam & MK_SHIFT)
					pressedKey |= VK_SHIFTBUTTON;
				param.pressedKey = static_cast<VirtualKeyCode>(pressedKey);
				pWindow->PostWindowMessage(WindowMessage::MOUSE_DOWN, param);
				break;
			}
			case WM_MOUSEMOVE:
			{
				MouseMoveParam param(pWindow);
				param.x = LOWORD(lParam);
				param.y = HIWORD(lParam);
				std::size_t pressedKey{ 0 };
				if (wParam & MK_CONTROL)
					pressedKey |= VK_CTRL;
				if (wParam & MK_LBUTTON)
					pressedKey |= VK_MOUSELBUTTON;
				if (wParam & MK_MBUTTON)
					pressedKey |= VK_MOUSEMBUTTON;
				if (wParam & MK_RBUTTON)
					pressedKey |= VK_MOUSERBUTTON;
				if (wParam & MK_SHIFT)
					pressedKey |= VK_SHIFTBUTTON;
				param.pressedKey = static_cast<VirtualKeyCode>(pressedKey);
				pWindow->PostWindowMessage(WindowMessage::MOUSE_MOVE, param);
				break;
			}
			case WM_CREATE:
				LOG_INFO("Win32 window created!");
				break;
			case WM_DESTROY:
				LOG_INFO("WM_DESTROY received!");
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
			return 0;
		}

		WinWindow::WinWindow():mCaption("Lightning Win32 Window")
			,mHwnd(nullptr), mWidth(800), mHeight(600)
		{
			HINSTANCE hInstance = ::GetModuleHandle(NULL);
			RegisterWindowClass(hInstance);
			CreateWindowInternal(hInstance);
			LOG_INFO("Create window succeed!");
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
			wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
			wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wndcls.lpszMenuName = NULL;
			wndcls.lpszClassName = sWindowClassName;
			wndcls.hIconSm = (HICON)::LoadIcon(hInstance, IDI_APPLICATION);

			if (!::RegisterClassEx(&wndcls))
			{
				throw WindowInitException("Register window class failed!ErrorCode : 0x%x", ::GetLastError());
			}
			LOG_INFO("Register window class succeed!");
		}

		void WinWindow::CreateWindowInternal(HINSTANCE hInstance)
		{
			RECT windowRect;
			windowRect.left = 0;
			windowRect.right = mWidth;
			windowRect.top = 0;
			windowRect.bottom = mHeight;
			::AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, NULL);
			LOG_DEBUG("Window width:%d, window height:%d", windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
			HWND hWnd = ::CreateWindowEx(0, sWindowClassName, mCaption.c_str(), 
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
				windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, static_cast<LPVOID>(this));

			if (!hWnd)
			{
				throw WindowInitException("Create window failed!ErrorCode : %x", ::GetLastError());
			}
		}


		WinWindow::~WinWindow()
		{
			if(mHwnd)
			{ 
				::DestroyWindow(mHwnd);
				mHwnd = nullptr;
			}
			LOG_INFO("Win32 window destructed!");
		}

		bool WinWindow::Show(bool show)
		{
			if (!mHwnd)
				return false;
			ShowWindow(mHwnd, show ? SW_SHOW : SW_HIDE);
			UpdateWindow(mHwnd);
			MSG msg{};
			while (true)
			{
				if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
					{
						mExitCode = msg.wParam;	
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
			return mWidth;
		}

		std::uint32_t WinWindow::GetHeight()const
		{
			return mHeight;
		}
		int WinWindow::GetDestroyCode()
		{
			return mExitCode;
		}
	}
}
