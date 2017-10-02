#include "common.h"
#include "win32application.h"
#include "d3d12\d3d12rendercontext.h"
#include "logger.h"
#include "windowmanager.h"
#include "filesystemfactory.h"

using LightningGE::Foundation::LogLevel;
using LightningGE::Foundation::logger;

namespace LightningGE
{
	namespace App
	{
		Win32Application::Win32Application()
		{
			m_filesystem = Foundation::FileSystemFactory::FileSystem();
			logger.Log(LogLevel::Info, "File system created!Current working directory:%s", m_filesystem->GetRoot().c_str());
			//Foundation::FilePtr file = m_filesystem->FindFile("test.txt");
			//if (file)
			//{
			//	Foundation::FileSize fileSize = file->GetSize();
			//	char* buffer = new char[fileSize+1];
			//	buffer[fileSize] = '\0';
			//	file->Read(buffer, fileSize);
			//	logger.Log(LogLevel::Debug, "File content is %s", buffer);
			//}
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
				m_pWin->Show(true);
				return 0;
			}
			return 1;
		}

		void Win32Application::Quit()
		{
			m_renderContext->ReleaseRenderResources();
			m_renderContext.reset();
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
			m_renderContext = std::shared_ptr<Renderer::IRenderContext>(new Renderer::D3D12RenderContext());
			return m_renderContext->Init(m_pWin);
		}

		void Win32Application::OnWindowIdle(const WindowSystem::WindowIdleParam& param)
		{
			m_renderContext->Render();
		}
	}
}
