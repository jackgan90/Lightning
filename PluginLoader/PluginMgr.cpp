#include "PluginMgr.h"

namespace Lightning
{
	namespace Lib
	{
		PluginMgr::PluginMgr()
		{

		}

		PluginPtr PluginMgr::Load(const std::string& name)
		{
			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				if (!it->second->IsLoaded())
					it->second->Load();
				return it->second;
			}
			auto plugin = PluginPtr(new Plugin(name));
			mPlugins.emplace(name, plugin);
			return plugin;
		}

		bool PluginMgr::Unload(const std::string& name)
		{
			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				return it->second->Unload();
			}
			return false;
		}

		bool PluginMgr::Unload(const PluginPtr& plugin)
		{
			if (plugin)
			{
				return plugin->Unload();
			}
			return false;
		}

		void PluginMgr::UnloadAll()
		{
			for (auto it = mPlugins.begin();it != mPlugins.end();++it)
			{
				it->second->Unload();
			}
		}
	}
}