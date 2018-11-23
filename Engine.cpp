#include "Engine.h"
#include "PlatformPlugin.h"
#include "FoundationPlugin.h"

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
		auto foundation = pluginMgr->LoadPlugin<Plugins::FoundationPlugin>("Foundation");
		auto configMgr = foundation->GetConfigManager();
		const auto& config = configMgr->GetConfig();
		for (auto it = config.Plugins.cbegin(); it != config.Plugins.cend();++it)
		{
			pluginMgr->LoadPlugin(*it);
		}
		auto platformPlugin = pluginMgr->GetPlugin<PlatformPlugin>("Platform");


		auto application = platformPlugin->CreateApplication();
		application->Start();

		while (application->IsRunning())
		{
			pluginMgr->Update();
		}

		auto exitCode = application->GetExitCode();

		for (auto it = config.Plugins.crbegin(); it != config.Plugins.crend();++it)
		{
			pluginMgr->UnloadPlugin(*it);
		}
		pluginMgr->UnloadPlugin("Foundation");
		DestroyPluginMgr(pluginMgr);
		return exitCode;
	}
}