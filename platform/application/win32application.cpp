#include "common.h"
#include "win32application.h"
#include "rendererfactory.h"
#include "logger.h"
#include "windowmanager.h"
#include "filesystemfactory.h"

namespace LightningGE
{
	namespace App
	{
		using Foundation::LogLevel;
		using Foundation::logger;
		using Foundation::FileSystemFactory;
		using Renderer::RendererFactory;
		using Renderer::IRenderer;
		Win32Application::Win32Application()
		{
			m_fs = FileSystemFactory::Instance()->CreateFileSystem();
			m_windowMgr = new WindowManager();
			logger.Log(LogLevel::Info, "File system created!Current working directory:%s", m_fs->GetRoot().c_str());
		}

		Win32Application::~Win32Application()
		{
			SAFE_DELETE(m_renderer);
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

		bool Win32Application::CreateMainWindow()
		{
			if (!m_window)
			{
				m_window = m_windowMgr->MakeWindow();
				if (!m_window)
				{
					return false;
				}
				m_window->Init();
			}
			return true;
		}

		void Win32Application::InitRenderContext()
		{
			m_renderer = RendererFactory::Instance()->CreateRenderer(m_window, m_fs);
		}

		void Win32Application::OnWindowIdle(const WindowSystem::WindowIdleParam& param)
		{
			m_renderer->Render();
		}
	}
}
