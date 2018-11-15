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
		auto platformPlugin = pluginMgr->Load<PlatformPlugin>("Platform");
		auto application = platformPlugin->CreateApplication();
		application->Start();

		while (application->IsRunning())
		{
			application->Update();
		}

		auto exitCode = application->GetExitCode();
		pluginMgr->UnloadAll();
		return exitCode;
	}
}