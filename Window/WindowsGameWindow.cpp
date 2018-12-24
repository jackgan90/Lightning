#include "WindowsGameWindow.h"

namespace Lightning
{
	namespace Window
	{
		LRESULT CALLBACK WindowsGameWindow::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (uMsg == WM_NCCREATE)
			{
				CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
				LPVOID pThis = pCS->lpCreateParams;
				SetWindowLongPtr(hwnd, 0, reinterpret_cast<LONG_PTR>(pThis));
				auto window = reinterpret_cast<WindowsGameWindow*>(pThis);
				window->mHwnd = hwnd;
				window->OnCreated();
				return TRUE;
			}
			auto window = reinterpret_cast<WindowsGameWindow*>(GetWindowLongPtrW(hwnd, 0));
			switch (uMsg)
			{
			case WM_SIZE:
			{
				RECT rect;
				::GetClientRect(hwnd, &rect);
				auto width = rect.right - rect.left;
				auto height = rect.bottom - rect.top;
				window->OnResize(std::size_t(width), std::size_t(height));
				break;
			}
			case WM_MOUSEWHEEL:
			{
				window->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, true);
				break;
			}
			case WM_KEYDOWN:
			{
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
				auto keyCode = static_cast<VirtualKeyCode>(code);
				window->OnKeyDown(keyCode);
				break;
			}
			case WM_RBUTTONDOWN:
			{
				std::size_t pressedKey = VK_MOUSERBUTTON;
				if (wParam & MK_CONTROL)
					pressedKey |= VK_CTRL;
				if (wParam & MK_LBUTTON)
					pressedKey |= VK_MOUSELBUTTON;
				if (wParam & MK_MBUTTON)
					pressedKey |= VK_MOUSEMBUTTON;
				if (wParam & MK_SHIFT)
					pressedKey |= VK_SHIFTBUTTON;
				auto keyCode = static_cast<VirtualKeyCode>(pressedKey);
				window->OnMouseDown(LOWORD(lParam), HIWORD(lParam), keyCode);
				break;
			}
			case WM_MOUSEMOVE:
			{
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
				auto keyCode = static_cast<VirtualKeyCode>(pressedKey);
				window->OnMouseMove(LOWORD(lParam), HIWORD(lParam), keyCode);
				break;
			}
			case WM_CREATE:
				LOG_INFO("Win32 window created!");
				break;
			case WM_DESTROY:
			{
				LOG_INFO("WM_DESTROY received!");
				PostQuitMessage(0);
				break;
			}
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
			return 0;
		}

		WindowsGameWindow::WindowsGameWindow() : mHwnd(nullptr)
		{
			mCaption = "Lightning Win32 Window";
			mWidth = 800;
			mHeight = 600;
			HINSTANCE hInstance = ::GetModuleHandle(NULL);
			RegisterWindowClass(hInstance);
			CreateWindowInternal(hInstance);
			LOG_INFO("Create window succeed!");
		}

		void WindowsGameWindow::RegisterWindowClass(HINSTANCE hInstance)
		{
			WNDCLASSEX wndcls;
			wndcls.cbSize = sizeof(WNDCLASSEX);
			wndcls.style = CS_HREDRAW | CS_VREDRAW;
			wndcls.lpfnWndProc = WndProc;
			wndcls.cbClsExtra = 0;
			wndcls.cbWndExtra = sizeof(WindowsGameWindow*);
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

		void WindowsGameWindow::CreateWindowInternal(HINSTANCE hInstance)
		{
			RECT windowRect;
			windowRect.left = 0;
			windowRect.right = mWidth;
			windowRect.top = 0;
			windowRect.bottom = mHeight;
			::AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, NULL);
			LOG_DEBUG("Window width:{0}, window height:{1}", windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
			HWND hWnd = ::CreateWindowEx(0, sWindowClassName, mCaption.c_str(), 
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
				windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, static_cast<LPVOID>(this));

			if (!hWnd)
			{
				throw WindowInitException("Create window failed!ErrorCode : %x", ::GetLastError());
			}
		}


		WindowsGameWindow::~WindowsGameWindow()
		{
			if(mHwnd)
			{ 
				::DestroyWindow(mHwnd);
				mHwnd = nullptr;
			}
			LOG_INFO("Win32 window destructed!");
		}

		void WindowsGameWindow::Update()
		{
			MSG msg{};
			if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message != WM_QUIT)
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				else
				{
					OnDestroy(int(msg.wParam));
				}
			}
			else
			{
				OnIdle();
			}
		}

		bool WindowsGameWindow::Show(bool show)
		{
			if (!mHwnd)
				return false;
			ShowWindow(mHwnd, show ? SW_SHOW : SW_HIDE);
			UpdateWindow(mHwnd);
			return true;
		}

	}
}
