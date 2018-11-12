#include "common.h"
#include "win32appsystem.h"
#include "rendererfactory.h"
#include "logger.h"
#include "scenemanager.h"
#include "ecs/systemmanager.h"
#include "ecs/eventmanager.h"
#include "winwindowsystem.h"
#include "timesystem.h"
#undef min
#undef max

namespace Lightning
{
	namespace App
	{
		using Render::RendererFactory;
		using namespace Window;
		using Scene::SceneManager;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Vector2i;
		using Foundation::SystemManager;

		Vector2i mousePosition;
		Win32AppSystem::Win32AppSystem()
		{
		}

		Win32AppSystem::~Win32AppSystem()
		{
		}

		void Win32AppSystem::Update(const EntityPtr& entity)
		{
			AppSystem::Update(entity);
		}

		void Win32AppSystem::RegisterWindowHandlers()
		{
			AppSystem::RegisterWindowHandlers();
			WINDOW_MSG_CLASS_HANDLER(MouseWheelEvent, OnMouseWheel);
			WINDOW_MSG_CLASS_HANDLER(KeyEvent, OnKeyDown);
			WINDOW_MSG_CLASS_HANDLER(MouseDownEvent, OnMouseDown);
			WINDOW_MSG_CLASS_HANDLER(MouseMoveEvent, OnMouseMove);
		}

		void Win32AppSystem::OnKeyDown(const Window::KeyEvent& event)
		{
			auto scene = SceneManager::Instance()->GetForegroundScene();
			if (scene)
			{
				auto camera = scene->GetActiveCamera();
				if (camera)
				{
					Vector3f camOffset;
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
					}
				}
			}

		}

		void Win32AppSystem::OnMouseDown(const Window::MouseDownEvent& event)
		{
			if (event.pressedKey & VK_MOUSERBUTTON)
			{
				mousePosition.x = static_cast<int>(event.x);
				mousePosition.y = static_cast<int>(event.y);
			}
		}

		void Win32AppSystem::OnMouseMove(const Window::MouseMoveEvent& event)
		{
			if (event.pressedKey & VK_MOUSERBUTTON)
			{
				auto scene = SceneManager::Instance()->GetForegroundScene();
				if (scene)
				{
					auto camera = scene->GetActiveCamera();
					if (camera)
					{
						float delta_x = float(event.x) - mousePosition.x;
						float delta_y = float(event.y) - mousePosition.y;
						Vector3f direction(delta_x, -delta_y, 0);
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


		void Win32AppSystem::OnMouseWheel(const MouseWheelEvent& event)
		{
			auto scene = SceneManager::Instance()->GetForegroundScene();
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

		SharedWindowPtr Win32AppSystem::CreateMainWindow()
		{
			return SystemManager::Instance()->CreateSystem<WinWindowSystem>();
		}

		UniqueRendererPtr Win32AppSystem::CreateRenderer()
		{
			return RendererFactory::Instance()->CreateRenderer(mAppComponent->window, mAppComponent->fileSystem);
		}
	}
}
