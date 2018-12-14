#pragma once
#include <memory>
#include "PluginMgrImpl/PluginManager.h"

namespace Lightning
{
	//Engine is designed such that it depends on no other module
	class Engine
	{
	public:
		static Engine* Instance()
		{
			static Engine sInstance;
			return &sInstance;
		}
		Plugins::IPluginManager* CreatePluginMgr()const
		{
			return new Plugins::PluginManager();
		}
		void DestroyPluginMgr(Plugins::IPluginManager* pluginMgr)
		{
			delete pluginMgr;
		}
		int Run();
	private:
		Engine();
	};
}
