#include "GameWindow.h"
#include "ECS/EventManager.h"

namespace Lightning
{
	namespace Window
	{
		using Foundation::EventManager;
		using Foundation::EntityManager;

		GameWindow::~GameWindow()
		{
		}

		GameWindow::GameWindow()
		{
		}

		void GameWindow::OnIdle()
		{
			WindowIdleEvent event(this);
			EventManager::Instance()->RaiseEvent<WindowIdleEvent>(event);
		}
	}
}