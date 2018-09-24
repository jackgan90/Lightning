#include "common.h"
#include "win32application.h"
#include "rendererfactory.h"
#include "logger.h"
#include "windowmanager.h"
#include "scenemanager.h"
#undef min
#undef max

namespace Lightning
{
	namespace App
	{
		using Render::RendererFactory;
		using namespace WindowSystem;
		using Scene::SceneManager;
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
				LOG_INFO("Win32Application start running!");
				//the call will block
				if(!mWindow->Show(true))
					return 0;
				return mWindow->GetDestroyCode();
			}
			return 0;
		}

		void Win32Application::RegisterWindowHandlers()
		{
			Application::RegisterWindowHandlers();
			auto window = GetMainWindow();
			if (window)
			{
				WINDOW_MSG_CLASS_HANDLER(window, WindowMessage::MOUSE_WHEEL, MouseWheelParam, OnMouseWheel);
			}
		}

		void Win32Application::OnMouseWheel(const MouseWheelParam& param)
		{
			auto scene = SceneManager::Instance()->GetForegroundScene();
			if (scene)
			{
				auto camera = scene->GetActiveCamera();
				if (camera)
				{
					auto fov = camera->GetFOV();
					fov -= param.wheel_delta;
					if (fov <= 0)
						fov = 1.0f;
					if (fov >= 180)
						fov = 180.0;
					camera->SetFOV(fov);
				}
			}
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
