#include "GameWindow.h"
#include "ECS/EventManager.h"
#include "IPluginManager.h"
#include "IFoundationPlugin.h"

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginManager* gPluginMgr;
	}
	namespace Window
	{
		GameWindow::GameWindow()
		{
			auto foundation = Plugins::GetPlugin<Plugins::IFoundationPlugin>(Plugins::gPluginMgr, "Foundation");
			mEventMgr = foundation->GetEventManager();
		}

		void GameWindow::OnIdle()
		{
			WindowIdleEvent event(this);
			mEventMgr->RaiseEvent(event);
		}
	}
}