#include "iapplication.h"


namespace LightningGE
{
	namespace App
	{
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
			auto pWin = GetMainWindow();
			if (pWin)
			{
				pWin->RegisterWindowMessageHandler(WindowSystem::MESSAGE_IDLE, 
					[&](WindowSystem::WindowMessage msg, const WindowSystem::WindowMessageParam& param) 
					{this->OnWindowIdle(reinterpret_cast<const WindowSystem::WindowIdleParam&>(param)); });
			}
		}
	}
}