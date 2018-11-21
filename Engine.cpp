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
		auto pluginMgr = GetPluginMgr();
		//Load Foundation.dll,this is the fundamental module providing basic services for all other plugins
		//such log, config ,file system etc.So it must be the first plugin to load and the last to free.
		pluginMgr->Load("Foundation");
		auto platformPlugin = pluginMgr->Load<PlatformPlugin>("Platform");
		auto application = platformPlugin->CreateApplication();
		application->Start();

		while (application->IsRunning())
		{
			application->Update();
		}

		auto exitCode = application->GetExitCode();
		pluginMgr->Unload("Window");
		pluginMgr->Unload("Platform");
		pluginMgr->Unload("Foundation");
		return exitCode;
	}
}