#include "common.h"
#include "win32application.h"
#include "rendererfactory.h"
#include "logger.h"
#include "windowmanager.h"

namespace LightningGE
{
	namespace App
	{
		using Foundation::LogLevel;
		using Foundation::logger;
		using Renderer::RendererFactory;
		Win32Application::Win32Application():Application()
		{
		}

		Win32Application::~Win32Application()
		{
		}

		int Win32Application::Run()
		{
			if (m_window)
			{
				logger.Log(LogLevel::Info, "Win32Application start running!");
				//the call will block
				if(!m_window->Show(true))
					return 0;
				return m_window->GetDestroyCode();
			}
			return 0;
		}

		WindowPtr Win32Application::CreateMainWindow()
		{
			return m_windowMgr->MakeWindow();
		}

		UniqueRendererPtr Win32Application::CreateRenderer()
		{
			return RendererFactory::Instance()->CreateRenderer(m_window, m_fs);
		}
	}
}
