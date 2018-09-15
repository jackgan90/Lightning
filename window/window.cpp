#include "window.h"

namespace Lightning
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
			if (m_msgHandlers.find(WindowMessage::IDLE) != m_msgHandlers.end())
			{
				m_msgHandlers[WindowMessage::IDLE](WindowMessage::IDLE, WindowIdleParam(this));
			}
		}
	}
}