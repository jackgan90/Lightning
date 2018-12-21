#include "Common.h"
#include "WindowsApplication.h"
#include "Logger.h"
#include "TimeSystem.h"
#include "IPluginManager.h"
#include "IRenderer.h"
#include "IScenePlugin.h"
#include "IFoundationPlugin.h"
#undef min
#undef max

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginManager* gPluginMgr;
	}
	namespace App
	{
		using namespace Window;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Vector2i;
		using Render::IRenderer;

		Vector2i mousePosition;

		void WindowsApplication::Start()
		{
			Application::Start();
			auto scenePlugin = Plugins::GetPlugin<Plugins::IScenePlugin>(Plugins::gPluginMgr, "Scene");
			mSceneMgr = scenePlugin->GetSceneManager();
		}

		void WindowsApplication::RegisterWindowHandlers()
		{
			Application::RegisterWindowHandlers();
			auto foundationPlugin = Plugins::GetPlugin<Plugins::IFoundationPlugin>(Plugins::gPluginMgr, "Foundation");
			auto eventMgr = foundationPlugin->GetEventManager();
			auto id = WINDOW_MSG_CLASS_HANDLER(eventMgr, MOUSE_WHEEL_EVENT, OnMouseWheel);
			mSubscriberIDs.insert(id);
			id = WINDOW_MSG_CLASS_HANDLER(eventMgr, KEYBOARD_EVENT, OnKeyDown);
			mSubscriberIDs.insert(id);
			id = WINDOW_MSG_CLASS_HANDLER(eventMgr, MOUSE_DOWN_EVENT, OnMouseDown);
			mSubscriberIDs.insert(id);
			id = WINDOW_MSG_CLASS_HANDLER(eventMgr, MOUSE_MOVE_EVENT, OnMouseMove);
			mSubscriberIDs.insert(id);
		}

		void WindowsApplication::OnKeyDown(const Foundation::IEvent& evt)
		{
			const auto& event = static_cast<const Window::KeyEvent&>(evt);
			auto scene = mSceneMgr->GetForegroundScene();
			if (scene)
			{
				auto camera = scene->GetActiveCamera();
				if (camera)
				{
					Vector3f camOffset{ 0, 0, 0 };
					auto position = camera->GetWorldPosition();
					switch (event.code & ~VK_CONTROL_MASK)
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
						camera->MoveTo(targetPosition);
						/*
						static std::size_t timerId{ 0 };
						static std::size_t repeatInterval = 1000 / 60;
						static long long now{ 0 };
						if (timerId)
						{
							mAppComponent->timer->RemoveTask(timerId);
						}
						auto pTimerId = &timerId;
						auto pNow = &now;
						now = Foundation::Time::Now();
						timerId = mAppComponent->timer->AddTask(Foundation::TimerTaskType::REPEAT, repeatInterval, repeatInterval, [pTimerId, pNow, targetPosition, camera, this]() {
							static float camSpeed{ 3.0f };
							auto now = Foundation::Time::Now();
							auto delta_time = (now - *pNow) / 1000.0f;
							*pNow = now;
							auto camPos = camera->GetWorldPosition();
							auto moveDir = targetPosition - camPos;
							auto distanceToTarget = moveDir.Length();
							if (distanceToTarget <= 0.001f)
							{
								mAppComponent->timer->RemoveTask(*pTimerId);
								return;
							}
							auto moveDistance = camSpeed * delta_time;
							if (moveDistance > distanceToTarget)
								moveDistance = distanceToTarget;
							moveDir.Normalize();
							camera->MoveTo(camPos + moveDir * moveDistance);
						});
						*/
					}
				}
			}

		}

		void WindowsApplication::OnMouseDown(const Foundation::IEvent& evt)
		{
			const auto& event = static_cast<const Window::MouseDownEvent&>(evt);
			if (event.pressedKey & VK_MOUSERBUTTON)
			{
				mousePosition.x = static_cast<int>(event.x);
				mousePosition.y = static_cast<int>(event.y);
			}
		}

		void WindowsApplication::OnMouseMove(const Foundation::IEvent& evt)
		{
			const auto& event = static_cast<const Window::MouseMoveEvent&>(evt);
			if (event.pressedKey & VK_MOUSERBUTTON)
			{
				auto scene = mSceneMgr->GetForegroundScene();
				if (scene)
				{
					auto camera = scene->GetActiveCamera();
					if (camera)
					{
						float delta_x = float(event.x) - mousePosition.x;
						float delta_y = float(event.y) - mousePosition.y;
						Vector3f direction{delta_x, -delta_y, 0};
						direction = camera->CameraDirectionToWorld(direction);
						auto forward = camera->GetForward();
						auto dest_dir = forward + direction * 0.005f;
						camera->RotateTowards(dest_dir);
					}
				}
				mousePosition.x = static_cast<int>(event.x);
				mousePosition.y = static_cast<int>(event.y);
			}
		}


		void WindowsApplication::OnMouseWheel(const Foundation::IEvent& evt)
		{
			const auto& event = static_cast<const MouseWheelEvent&>(evt);
			auto scene = mSceneMgr->GetForegroundScene();
			if (scene)
			{
				auto camera = scene->GetActiveCamera();
				if (camera)
				{
					auto fov = camera->GetFOV();
					fov -= event.wheel_delta;
					if (fov <= 0)
						fov = 1.0f;
					if (fov >= 180)
						fov = 180.0;
					camera->SetFOV(fov);
				}
			}
		}
	}
}
