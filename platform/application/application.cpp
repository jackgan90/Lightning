#include "iapplication.h"
#include "logger.h"


namespace LightningGE
{
	namespace App
	{
		using Foundation::logger;
		using Foundation::LogLevel;

		Application::Application()
		{
			logger.Log(LogLevel::Info, "Application initialized successfully!");
		}

		Application::~Application()
		{
			logger.Log(LogLevel::Info, "Application quit.");
		}

		void Application::Start()
		{
			CreateMainWindow();
			InitRenderContext();
			RegisterWindowHandlers();
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