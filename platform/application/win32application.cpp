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
		using Foundation::Math::Vector3f;
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
				WINDOW_MSG_CLASS_HANDLER(window, WindowMessage::KEY_DOWN, KeyParam, OnKeyDown);
			}
		}

		void Win32Application::OnKeyDown(const WindowSystem::KeyParam& param)
		{
			static std::size_t cameraMoveTimerId{ 0 };
			static float progress{ 0.0f };
			auto scene = SceneManager::Instance()->GetForegroundScene();
			if (scene)
			{
				auto camera = scene->GetActiveCamera();
				if (camera)
				{
					auto position = camera->GetWorldPosition();
					Vector3f offset{ 0, 0, 0 };
					switch (param.code)
					{
					case VK_A:
						offset = Vector3f::left();
						break;
					case VK_D:
						offset = Vector3f::right();
						break;
					case VK_W:
						offset = Vector3f::back();
						break;
					case VK_S:
						offset = Vector3f::forward();
						break;
					case VK_Q:
						offset = Vector3f::up();
						break;
					case VK_E:
						offset = Vector3f::down();
						break;
					default:
						break;
					}
					if (!offset.IsZero())
					{
						mTimer->RemoveTask(cameraMoveTimerId);
						offset *= 0.2f;
						auto targetPosition = position + offset;
						std::size_t *ptrTimerID = &cameraMoveTimerId;
						float* ptrProgress = &progress;
						progress = 0.0f;
						cameraMoveTimerId = mTimer->AddTask(Foundation::TimerTaskType::REPEAT, 15, 15, 
							[ptrTimerID, ptrProgress, camera, position, targetPosition, this]() {
							if (*ptrProgress >= 1.0f)
								mTimer->RemoveTask(*ptrTimerID);
							else
							{
								*ptrProgress += 0.1f;
								auto x = *ptrProgress;
								x = x * x * x * (x * (x * 6 - 15) + 10);
								camera->SetWorldPosition(position * (1 - x) + targetPosition * x);
							}
						});
					}
				}
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
