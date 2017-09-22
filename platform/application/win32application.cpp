#include "common.h"
#include "win32application.h"
#include "logger.h"
#include "windowmanager.h"
#include "filesystemfactory.h"

using LogLevel = LightningGE::Utility::LogLevel;
using LightningGE::Utility::logger;

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
