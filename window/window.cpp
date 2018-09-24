#include "window.h"

namespace Lightning
{
	namespace WindowSystem
	{
		void Window::RegisterWindowMessageHandler(WindowMessage msg, WindowMessageHandler handler)
		{
			if (mMsgHandlers.find(msg) == mMsgHandlers.end())
			{
				mMsgHandlers[msg] = handler;
			}
		}

		void Window::OnIdle()
		{
			PostWindowMessage(WindowMessage::IDLE, WindowIdleParam(this));
		}
	}
}