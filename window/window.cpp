#include "window.h"
#include "ecs/event.h"

namespace Lightning
{
	namespace WindowSystem
	{
		using Foundation::EventManager;
		void Window::OnIdle()
		{
			WindowIdleEvent event(this);
			EventManager::Instance()->RaiseEvent<WindowIdleEvent>(event);
		}
	}
}