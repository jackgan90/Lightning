#pragma once
#include "Plugin.h"
#include "Logger.h"
#include "ECS/IEventManager.h"
#include "IFileSystem.h"
#include "IConfigManager.h"
#include "IEnvironment.h"

#define INIT_LOGGER(mgr, name)										\
{																		\
	mgr->LoadPlugin<Lightning::Plugins::FoundationPlugin>("Foundation")		\
	->InitLogger(#name, Foundation::Logger::Instance());				\
}

#define FINALIZE_LOGGER(mgr) \
mgr->GetPlugin<Lightning::Plugins::FoundationPlugin>("Foundation")		\
->FinalizeLogger(Foundation::Logger::Instance());				\
mgr->UnloadPlugin("Foundation");


namespace Lightning
{
	namespace Plugins
	{
		class FoundationPlugin : public Plugin
		{
		public:
			virtual void InitLogger(const char* name, Foundation::Logger* logger) = 0;
			virtual void FinalizeLogger(Foundation::Logger* logger) = 0;
			virtual Foundation::IEventManager* GetEventManager() = 0;
			virtual Foundation::IFileSystem* GetFileSystem() = 0;
			virtual Foundation::IConfigManager* GetConfigManager() = 0;
			virtual Foundation::IEnvironment* GetEnvironment() = 0;
		};
	}
}