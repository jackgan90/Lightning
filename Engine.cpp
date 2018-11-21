#include "Engine.h"
#include "PlatformPlugin.h"

namespace Lightning
{
	using Plugins::PlatformPlugin;
	Engine::Engine()
	{

	}

	int Engine::Run()
	{
		auto pluginMgr = CreatePluginMgr();
		//Load Foundation.dll,this is the fundamental module providing basic services for all other plugins
		//such log, config ,file system etc.So it must be the first plugin to load and the last to free.
		pluginMgr->LoadPlugin("Foundation");
		auto platformPlugin = pluginMgr->LoadPlugin<PlatformPlugin>("Platform");
		auto application = platformPlugin->CreateApplication();
		application->Start();

		while (application->IsRunning())
		{
			application->Update();
			pluginMgr->Update();
		}

		auto exitCode = application->GetExitCode();
		pluginMgr->UnloadPlugin("Window");
		pluginMgr->UnloadPlugin("Platform");
		pluginMgr->UnloadPlugin("Foundation");
		DestroyPluginMgr(pluginMgr);
		return exitCode;
	}
}