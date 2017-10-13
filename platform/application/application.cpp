#include "common.h"
#include "filesystemfactory.h"
#include "iapplication.h"
#include "logger.h"


namespace LightningGE
{
	namespace App
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		using Foundation::FileSystemFactory;
		Application::Application()
		{
			m_fs = FileSystemFactory::Instance()->CreateFileSystem();
			m_windowMgr = std::make_unique<WindowManager>();
			logger.Log(LogLevel::Info, "File system created!Current working directory:%s", m_fs->GetRoot().c_str());
			logger.Log(LogLevel::Info, "Application initialized successfully!");
		}

		Application::~Application()
		{
			m_renderer.reset();
			m_windowMgr.reset();
			logger.Log(LogLevel::Info, "Application quit.");
		}

		void Application::Start()
		{
			m_window = CreateMainWindow();
			m_renderer = CreateRenderer();
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

		void Application::OnWindowIdle(const WindowSystem::WindowIdleParam& param)
		{
			if(m_renderer)
				m_renderer->Render();
		}


	}
}