#pragma once
#include "Logger.h"
#include "IPlugin.h"
#include "IFileSystem.h"
#include "IConfigManager.h"
#include "IEnvironment.h"

#define INIT_LOGGER(mgr, name)										\
{																		\
	Lightning::Plugins::GetPlugin<Lightning::Plugins::IFoundationPlugin>(mgr, "Foundation")		\
	->InitLogger(#name, Foundation::Logger::Instance());				\
}

#define FINALIZE_LOGGER(mgr) \
Lightning::Plugins::GetPlugin<Lightning::Plugins::IFoundationPlugin>(mgr, "Foundation")		\
->FinalizeLogger(Foundation::Logger::Instance());				\
mgr->UnloadPlugin("Foundation");


namespace Lightning
{
	namespace Plugins
	{
		struct IFoundationPlugin : public IPlugin
		{
			virtual void INTERFACECALL InitLogger(const char* name, Foundation::Logger* logger) = 0;
			virtual void INTERFACECALL FinalizeLogger(Foundation::Logger* logger) = 0;
			virtual Foundation::IFileSystem* INTERFACECALL GetFileSystem() = 0;
			virtual Foundation::IConfigManager* INTERFACECALL GetConfigManager() = 0;
			virtual Foundation::IEnvironment* INTERFACECALL GetEnvironment() = 0;
		};
	}
}