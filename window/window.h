#pragma once
#include "iwindow.h"

namespace Lightning
{
	namespace WindowSystem
	{
		class LIGHTNING_WINDOW_API Window : public IWindow
		{
			void RegisterWindowMessageHandler(WindowMessage msg, WindowMessageHandler handler)override;
		protected:
			virtual void OnIdle();
			std::unordered_map<WindowMessage, WindowMessageHandler> mMsgHandlers;
		};
	}
}
