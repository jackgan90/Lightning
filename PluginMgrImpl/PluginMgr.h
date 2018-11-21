#pragma once
#include <string>
#include <mutex>
#include <unordered_map>
#include <tuple>
#include "IPluginMgr.h"
#ifdef LIGHTNING_WIN32
#include <Windows.h>
#endif


namespace Lightning
{
	class Engine;
	namespace Plugins
	{
		//An instance of a PluginMgr can only be created by Engine.
		class PluginMgr : public IPluginMgr
		{
		public:
			Plugin* Load(const std::string& pluginName)override;
			Plugin* GetPlugin(const std::string& pluginName)override;
			bool Unload(const std::string& pluginName)override;
			void Update()override;
		private:
			friend class Engine;
			struct PluginInfo
			{
				Plugin* plugin;
#ifdef LIGHTNING_WIN32
				HMODULE handle;
#endif
			};
			using PluginTable = std::unordered_map<std::string, PluginInfo>;
			PluginMgr();
			~PluginMgr();
			PluginTable mPlugins;
			std::recursive_mutex mPluginsMutex;
			static constexpr char* GET_PLUGIN_PROC = "GetPlugin";
		};
	}
}