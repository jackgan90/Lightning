#include "GameWindow.h"
#include "ECS/EventManager.h"
#include "IPluginManager.h"
#include "FoundationPlugin.h"

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
			auto foundationPlugin = Lightning::Plugins::gPluginMgr->GetPlugin<Lightning::Plugins::FoundationPlugin>("Foundation");
			mEventMgr = foundationPlugin->GetEventManager();
		}

		void GameWindow::OnIdle()
		{
			WindowIdleEvent event(this);
			mEventMgr->RaiseEvent(event);
		}
	}
}