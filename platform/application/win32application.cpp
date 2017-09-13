#include "win32application.h"
#include "logging/logger.h"
#include "../window/windowmanager.h"

using LogLevel = LightningGE::Utility::LogLevel;
using LightningGE::Utility::logger;

namespace LightningGE
{
	namespace App
	{
		Win32Application::Win32Application()
		{

		}

		Win32Application::~Win32Application()
		{
			TryDestroyWindow();
		}

		void Win32Application::TryDestroyWindow()
		{
			if (m_pWin)
			{
				m_pWin->destroy();
				m_pWin.reset();
			}
		}

		bool Win32Application::Init()
		{
			logger.Log(LogLevel::Info, "Win32Application initialized successfully!");
			return true;
		}

		bool Win32Application::Start()
		{
			logger.Log(LogLevel::Info, "Win32Application start successfully!");
			using WindowSystem::WindowManager;
			if (!m_pWin)
			{
				m_pWin = WindowManager::Instance()->MakeWindow();
				if (!m_pWin)
				{
					return false;
				}
				m_pWin->Init();
			}
			return true;
		}

		int Win32Application::Run()
		{
			if (m_pWin)
			{
				logger.Log(LogLevel::Info, "Win32Application start running!");
				m_pWin->Show(true);
				return 0;
			}
			return 1;
		}

		void Win32Application::Quit()
		{
			TryDestroyWindow();
			logger.Log(LogLevel::Info, "Win32Application quit!");
		}
	}
}
