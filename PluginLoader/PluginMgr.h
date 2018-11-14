#pragma once
#include <string>
#include <mutex>
#include "Plugin.h"
#include "Container.h"

namespace Lightning
{
	class Engine;
	namespace Plugins
	{
		using Foundation::Container;
		//An instance of a PluginMgr can only be created by Engine.
		class PluginMgr
		{
		public:
			Plugin* Load(const std::string& pluginName);
			Plugin* GetPlugin(const std::string& pluginName);
			bool Unload(const std::string& pluginName);
			void UnloadAll();
		private:
			friend class Engine;
			struct PluginInfo
			{
				Plugin* pPlugin;
#ifdef LIGHTNING_WIN32
				HMODULE handle;
#endif
			};
			PluginMgr();
			Container::UnorderedMap<std::string, PluginInfo> mPlugins;
			std::recursive_mutex mPluginsMutex;
			static constexpr char* GET_PLUGIN_FUNC = "GetPlugin";
		};
	}
}