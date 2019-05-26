#include "Common.h"
#include "WindowsApplication.h"
#include "Logger.h"
#include "TimeSystem.h"
#include "IPluginManager.h"
#include "IRenderer.h"
#include "IWorldPlugin.h"
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
			auto worldPlugin = Plugins::GetPlugin<Plugins::IWorldPlugin>(Plugins::gPluginMgr, "World");
			mSceneMgr = worldPlugin->GetSceneManager();
		}

		void WindowsApplication::OnWindowMouseWheel(IWindow* window, int delta, bool isVertical)
		{
			auto scene = mSceneMgr->GetForegroundScene();
			if (scene)
			{
				auto camera = scene->GetActiveCamera();
				if (camera)
				{
					auto fov = camera->GetFOV();
					fov -= delta;
					if (fov <= 0)
						fov = 1.0f;
					if (fov >= 180)
						fov = 180.0;
					camera->SetFOV(fov);
				}
			}
		}

		void WindowsApplication::OnWindowKeyDown(IWindow* window, VirtualKeyCode keyCode)
		{
			auto scene = mSceneMgr->GetForegroundScene();
			if (scene)
			{
				auto camera = scene->GetActiveCamera();
				if (camera)
				{
					Vector3f camOffset{ 0, 0, 0 };
					auto& cameraTransform = camera->GetLocalTransform();
					auto position = cameraTransform.GetPosition();
					switch (keyCode & ~VK_CONTROL_MASK)
					{
					case VK_A:
						camOffset += Vector3f::left();
						break;
					case VK_D:
						camOffset += Vector3f::right();
						break;
					case VK_W:
						camOffset += Vector3f::forward();
						break;
					case VK_S:
						camOffset += Vector3f::back();
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
						auto position = cameraTransform.GetPosition();
						camOffset.Normalize();
						camOffset *= 0.3f;
						camOffset = cameraTransform.LocalDirectionToGlobal(camOffset);
						auto targetPosition = position + camOffset;
						cameraTransform.SetPosition(targetPosition);
					}
				}
			}

		}

		void WindowsApplication::OnWindowMouseDown(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)
		{
			if (keyCode & VK_MOUSERBUTTON)
			{
				mousePosition.x = static_cast<int>(x);
				mousePosition.y = static_cast<int>(y);
			}
		}

		void WindowsApplication::OnWindowMouseMove(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)
		{
			if (keyCode & VK_MOUSERBUTTON)
			{
				auto scene = mSceneMgr->GetForegroundScene();
				if (scene)
				{
					auto camera = scene->GetActiveCamera();
					if (camera)
					{
						float delta_x = float(x) - mousePosition.x;
						float delta_y = float(y) - mousePosition.y;
						Vector3f direction{delta_x, -delta_y, 0};
						auto& cameraTransform = camera->GetLocalTransform();
						direction = cameraTransform.LocalDirectionToGlobal(direction);
						auto forward = cameraTransform.Forward();
						auto dest_dir = forward + direction * 0.005f;
						//camera->RotateTowards(dest_dir);
						cameraTransform.OrientTo(dest_dir);
						////FIXME : Due to limited precision of float,the resulting forward after transformation
						////may invert the direction and points to the opposite direction
						////If this happens,just revert to previous forward
						//auto newForward = camera->GetForward();
						//if (forward.Dot(newForward) < 0)
						//	camera->RotateTowards(forward);
					}
				}
				mousePosition.x = static_cast<int>(x);
				mousePosition.y = static_cast<int>(y);
			}
		}
	}
}
