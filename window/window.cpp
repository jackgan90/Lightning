#include "window.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		void Window::RegisterWindowMessageHandler(WindowMessage msg, WindowMessageHandler handler)
		{
			if (m_msgHandlers.find(msg) == m_msgHandlers.end())
			{
				m_msgHandlers[msg] = handler;
			}
		}

		void Window::OnIdle()
		{
			if (m_msgHandlers.find(MESSAGE_IDLE) != m_msgHandlers.end())
			{
				m_msgHandlers[MESSAGE_IDLE](MESSAGE_IDLE, WindowIdleParam( GetNativeHandle()));
			}
		}
	}
}