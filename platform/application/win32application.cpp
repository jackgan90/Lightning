#include "common.h"
#include "win32application.h"
#include "rendererfactory.h"
#include "logger.h"
#include "windowmanager.h"
#include "scenemanager.h"
#include "timesystem.h"
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
		using Foundation::Math::Vector2i;

		Vector2i mousePosition;
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
				WINDOW_MSG_CLASS_HANDLER(window, WindowMessage::MOUSE_DOWN, MouseDownParam, OnMouseDown);
				WINDOW_MSG_CLASS_HANDLER(window, WindowMessage::MOUSE_MOVE, MouseMoveParam, OnMouseMove);
			}
		}

		void Win32Application::OnKeyDown(const WindowSystem::KeyParam& param)
		{
			auto scene = SceneManager::Instance()->GetForegroundScene();
			if (scene)
			{
				auto camera = scene->GetActiveCamera();
				if (camera)
				{
					Vector3f camOffset;
					auto position = camera->GetWorldPosition();
					switch (param.code & ~VK_CONTROL_MASK)
					{
					case VK_A:
						camOffset += Vector3f::left();
						break;
					case VK_D:
						camOffset += Vector3f::right();
						break;
					case VK_W:
						camOffset += Vector3f::back();
						break;
					case VK_S:
						camOffset += Vector3f::forward();
						break;
					case VK_Q:
						camOffset += Vector3f::up();
						break;
					case VK_E:
						camOffset += Vector3f::down();
						break;
					default:
						break;
					}
					if (!camOffset.IsZero())
					{
						auto position = camera->GetWorldPosition();
						camOffset.Normalize();
						camOffset *= 0.3f;
						camOffset = camera->CameraDirectionToWorld(camOffset);
						auto targetPosition = position + camOffset;
						static std::size_t timerId{ 0 };
						static std::size_t repeatInterval = 1000 / 60;
						static std::size_t now{ 0 };
						if (timerId)
						{
							mTimer->RemoveTask(timerId);
						}
						auto pTimerId = &timerId;
						auto pNow = &now;
						now = Foundation::Time::Now();
						timerId = mTimer->AddTask(Foundation::TimerTaskType::REPEAT, repeatInterval, repeatInterval, [pTimerId, pNow, targetPosition, camera, this]() {
							static float camSpeed{ 3.0f };
							auto now = Foundation::Time::Now();
							auto delta_time = (now - *pNow) / 1000.0f;
							*pNow = now;
							auto camPos = camera->GetWorldPosition();
							auto moveDir = targetPosition - camPos;
							auto distanceToTarget = moveDir.Length();
							if (distanceToTarget <= 0.001f)
							{
								mTimer->RemoveTask(*pTimerId);
								return;
							}
							auto moveDistance = camSpeed * delta_time;
							if (moveDistance > distanceToTarget)
								moveDistance = distanceToTarget;
							moveDir.Normalize();
							camera->SetWorldPosition(camPos + moveDir * moveDistance);
						});
					}
				}
			}

		}

		void Win32Application::OnMouseDown(const WindowSystem::MouseDownParam& param)
		{
			if (param.pressedKey & VK_MOUSERBUTTON)
			{
				mousePosition.x = param.x;
				mousePosition.y = param.y;
			}
		}

		void Win32Application::OnMouseMove(const WindowSystem::MouseMoveParam& param)
		{
			if (param.pressedKey & VK_MOUSERBUTTON)
			{
				auto scene = SceneManager::Instance()->GetForegroundScene();
				if (scene)
				{
					auto camera = scene->GetActiveCamera();
					if (camera)
					{
						float delta_x = float(param.x) - mousePosition.x;
						float delta_y = float(param.y) - mousePosition.y;
						Vector3f direction(delta_x, -delta_y, 0);
						direction = camera->CameraDirectionToWorld(direction);
						auto forward = camera->GetForward();
						camera->RotateTowards(forward + direction * 0.005f);
					}
				}
				mousePosition.x = param.x;
				mousePosition.y = param.y;
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
