#include "WindowSystem.h"
#include "ecs/entitymanager.h"
#include "ecs/eventmanager.h"
#include "rttr/detail/registration/registration_manager.h"

namespace Lightning
{
	namespace Window
	{
		using Foundation::EventManager;
		using Foundation::EntityManager;

		WindowSystem::~WindowSystem()
		{
			rttr::detail::get_registration_manager().unregister();
		}

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