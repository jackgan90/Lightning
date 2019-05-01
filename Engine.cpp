#include "Engine.h"
#include "IPlatformPlugin.h"
#include "IFoundationPlugin.h"

namespace Lightning
{
	using Plugins::IPlatformPlugin;
	Engine::Engine()
	{

	}

	int Engine::Run()
	{
		auto pluginMgr = CreatePluginMgr();
		//Load Foundation.dll,this is the fundamental module providing basic services for all other plugins
		//such log, config ,file system etc.So it must be the first plugin to load and the last to free.
		auto foundation = Plugins::LoadPlugin<Plugins::IFoundationPlugin>(pluginMgr, "Foundation");
		auto configMgr = foundation->GetConfigManager();
		const auto& config = configMgr->GetConfig();
		for (unsigned i = 0;i < config.Plugins.size();++i)
		{
			pluginMgr->LoadPlugin(config.Plugins[i]);
		}
		auto platformPlugin = Plugins::GetPlugin<IPlatformPlugin>(pluginMgr, "Platform");


		auto application = platformPlugin->CreateApplication();
		application->Start();

		while (application->IsRunning())
		{
			pluginMgr->Update();
		}

		auto exitCode = application->GetExitCode();

		for (unsigned i = 0;i < config.Plugins.size();++i)
		{
			pluginMgr->UnloadPlugin(config.Plugins[i]);
		}
		pluginMgr->UnloadPlugin("Foundation");
		DestroyPluginMgr(pluginMgr);
		return exitCode;
	}
}