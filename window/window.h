#pragma once
#include "iwindow.h"

namespace Lightning
{
	namespace WindowSystem
	{
		class LIGHTNING_WINDOW_API Window : public IWindow
		{
		public:
			void RegisterWindowMessageHandler(WindowMessage msg, WindowMessageHandler handler)override;
			void PostWindowMessage(WindowMessage msg, const WindowMessageParam& param)override
			{
				auto it = mMsgHandlers.find(msg);
				if (it != mMsgHandlers.end())
				{
					it->second(msg, param);
				}
			}
		protected:
			virtual void OnIdle();
			std::unordered_map<WindowMessage, WindowMessageHandler> mMsgHandlers;
		};
	}
}
