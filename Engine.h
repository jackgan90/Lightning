#pragma once
#include <memory>
#include "PluginMgrImpl/PluginMgr.h"

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
		Plugins::IPluginMgr* CreatePluginMgr()const
		{
			return new Plugins::PluginMgr();
		}
		void DestroyPluginMgr(Plugins::IPluginMgr* pluginMgr)
		{
			delete pluginMgr;
		}
		int Run();
	private:
		Engine();
	};
}
