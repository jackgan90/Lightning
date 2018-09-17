#include "common.h"
#include "win32application.h"
#include "rendererfactory.h"
#include "logger.h"
#include "windowmanager.h"

namespace Lightning
{
	namespace App
	{
		using Foundation::LogLevel;
		using Foundation::logger;
		using Render::RendererFactory;
		Win32Application::Win32Application():Application()
		{
		}

		Win32Application::~Win32Application()
		{
		}

		int Win32Application::Run()
		{
			if (mWindow)
			{
				logger.Log(LogLevel::Info, "Win32Application start running!");
				//the call will block
				if(!mWindow->Show(true))
					return 0;
				return mWindow->GetDestroyCode();
			}
			return 0;
		}

		SharedWindowPtr Win32Application::CreateMainWindow()
		{
			return mWindowMgr->MakeWindow();
		}

		UniqueRendererPtr Win32Application::CreateRenderer()
		{
			return RendererFactory::Instance()->CreateRenderer(mWindow, mFileSystem);
		}
	}
}
