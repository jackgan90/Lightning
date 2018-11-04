#include "windowsystem.h"
#include "ecs/entitymanager.h"
#include "ecs/event.h"

namespace Lightning
{
	namespace Window
	{
		using Foundation::EventManager;
		using Foundation::EntityManager;

		WindowSystem::WindowSystem() : System(WindowSystemPriority)
		{
			mEntity = EntityManager::Instance()->CreateEntity<WindowEntity>();
			mComponent = mEntity->AddComponent<WindowComponent>();
		}

		void WindowSystem::OnIdle()
		{
			WindowIdleEvent event(this);
			EventManager::Instance()->RaiseEvent<WindowIdleEvent>(event);
		}
	}
}