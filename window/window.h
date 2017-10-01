#pragma once
#include "iwindow.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		class LIGHTNINGGE_WINDOW_API Window : public IWindow
		{
			void RegisterWindowMessageHandler(WindowMessage msg, WindowMessageHandler handler)override;
		protected:
			virtual void OnIdle();
			std::unordered_map<WindowMessage, WindowMessageHandler> m_msgHandlers;
		};
	}
}
