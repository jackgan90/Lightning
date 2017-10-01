#include "iapplication.h"


namespace LightningGE
{
	namespace App
	{
		using namespace WindowSystem;
		bool Application::Start()
		{
			bool result = true;
			result &= CreateMainWindow();
			if(result)
				result &= InitRenderContext();
			if (result)
				RegisterWindowHandlers();
			return result;
		}

		void Application::RegisterWindowHandlers()
		{
			WindowPtr pWin = GetMainWindow();
			if (pWin)
			{
				pWin->RegisterWindowMessageHandler(MESSAGE_IDLE, 
					[&](WindowMessage msg, const WindowMessageParam& param) {this->OnWindowIdle(reinterpret_cast<const WindowIdleParam&>(param)); });
			}
		}
	}
}