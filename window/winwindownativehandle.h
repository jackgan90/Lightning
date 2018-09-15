#pragma once
#include <Windows.h>
#include "iwindownativehandle.h"

namespace Lightning
{
	namespace WindowSystem
	{
		class LIGHTNING_WINDOW_API WinWindowNativeHandle : public IWindowNativeHandle
		{
		public:
			friend class WinWindow;
			WinWindowNativeHandle(HWND hWnd);
			~WinWindowNativeHandle()override;
			bool operator==(const IWindowNativeHandle& handle)override;
			NativeHandleIdentifier GetIdentifier()const override;
			bool OwnWindowsHandle(HWND hWnd)const { return m_hWnd == hWnd;}
			operator HWND() { return m_hWnd; }

		private:
			HWND m_hWnd;
		};
	}
}
