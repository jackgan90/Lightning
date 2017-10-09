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
		using Renderer::IRenderContext;
		using Renderer::IRenderer;
		using Renderer::IRenderTargetManager;
		using Renderer::IShaderManager;
		Win32Application::Win32Application()
		{
			logger.Log(LogLevel::Info, "File system created!Current working directory:%s", FileSystemFactory::Instance()->FileSystem()->GetRoot().c_str());
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
			bool result = Application::Start();
			if(result)
				logger.Log(LogLevel::Info, "Win32Application start successfully!");
			return result;
		}

		int Win32Application::Run()
		{
			if (m_pWin)
			{
				logger.Log(LogLevel::Info, "Win32Application start running!");
				if(!m_pWin->Show(true));
					return 0;
				return m_pWin->GetDestroyCode();
			}
			return 0;
		}

		void Win32Application::Quit()
		{
			m_renderer->ReleaseRenderResources();
			RendererFactory<IRenderContext>::Instance()->Finalize();
			RendererFactory<IRenderer>::Instance()->Finalize();
			RendererFactory<IRenderTargetManager>::Instance()->Finalize();
			RendererFactory<IShaderManager>::Instance()->Finalize();
			FileSystemFactory::Instance()->Finalize();
			TryDestroyWindow();
			logger.Log(LogLevel::Info, "Win32Application quit!");
		}

		bool Win32Application::CreateMainWindow()
		{
			if (!m_pWin)
			{
				m_pWin = WindowSystem::WindowManager::Instance()->MakeWindow();
				if (!m_pWin)
				{
					return false;
				}
				m_pWin->Init();
			}
			return true;
		}

		bool Win32Application::InitRenderContext()
		{
			auto pContext = RendererFactory<IRenderContext>::Instance()->Create();
			if (!pContext)
				return false;
			bool result = pContext->Init(m_pWin);
			if (result)
			{
				m_renderer = RendererFactory<IRenderer>::Instance()->Create(pContext);
			}
			return result;
		}

		void Win32Application::OnWindowIdle(const WindowSystem::WindowIdleParam& param)
		{
			m_renderer->Render();
		}
	}
}
