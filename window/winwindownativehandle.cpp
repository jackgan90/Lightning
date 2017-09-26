#include "winwindownativehandle.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		WinWindowNativeHandle::WinWindowNativeHandle(HWND hWnd):m_hWnd(hWnd)
		{

		}

		WinWindowNativeHandle::~WinWindowNativeHandle()
		{
			m_hWnd = nullptr;
		}

		NativeHandleIdentifier WinWindowNativeHandle::GetIdentifier()const
		{
			return IDENTIFIER_WINDOWS_HANDLE;
		}
		
		bool WinWindowNativeHandle::operator==(const IWindowNativeHandle& handle)
		{
			if (GetIdentifier() != handle.GetIdentifier())
				return false;
			auto otherHandle = reinterpret_cast<const WinWindowNativeHandle*>(&handle);
			return m_hWnd == otherHandle->m_hWnd;
		}
	}
}