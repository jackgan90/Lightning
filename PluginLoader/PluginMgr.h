#pragma once
#include <string>
#include "PluginExportDef.h"
#include "Plugin.h"
#include "singleton.h"
#include "container.h"

namespace Lightning
{
	namespace Lib
	{
		using Foundation::Container;
		class LIGHTNING_PLUGIN_API PluginMgr : Foundation::Singleton<PluginMgr>
		{
		public:
			PluginPtr Load(const std::string& pluginName);
			bool Unload(const std::string& pluginName);
			bool Unload(const PluginPtr& pluginName);
			void UnloadAll();
		private:
			friend class Foundation::Singleton<PluginMgr>;
			PluginMgr();
			Container::UnorderedMap<std::string, PluginPtr> mPlugins;
		};
	}
}