#pragma once
#include "Plugin.h"
#include "Logger.h"
#include "ECS/IEventManager.h"
#include "IFileSystem.h"

#define INIT_LOGGER(mgr, name)										\
{																		\
	mgr->Load<Lightning::Plugins::FoundationPlugin>("Foundation")		\
	->InitLogger(#name, Foundation::Logger::Instance());				\
}

#define UNLOAD_FOUNDATION(mgr) mgr->Unload("Foundation");


namespace Lightning
{
	namespace Plugins
	{
		class FoundationPlugin : public Plugin
		{
		public:
			virtual void InitLogger(const char* name, Foundation::Logger* logger) = 0;
			virtual Foundation::IEventManager* GetEventManager() = 0;
			virtual Foundation::IFileSystem* CreateFileSystem() = 0;
		};
	}
}