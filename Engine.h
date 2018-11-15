#pragma once
#include <memory>
#include "PluginLoader/PluginMgr.h"
#include "Application/Application.h"

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
		Plugins::PluginMgr* GetPluginMgr()const
		{
			static Plugins::PluginMgr sPluginMgr;
			return &sPluginMgr;
		}
		int Run();
	private:
		Engine();
		std::unique_ptr<App::Application> mApplication;
		bool mRunning;
	};
}