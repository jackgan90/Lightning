#include "PluginMgr.h"

namespace Lightning
{
	namespace Plugins
	{
		PluginMgr::PluginMgr()
		{

		}

		Plugin* PluginMgr::Load(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				it->second.pPlugin->AddRef();
				return it->second.pPlugin;
			}
			PluginInfo info;
#ifdef LIGHTNING_WIN32
			info.handle = ::LoadLibrary(name.c_str());
			if (info.handle)
			{
				GetPluginProc pFunc = (GetPluginProc)::GetProcAddress(info.handle, GET_PLUGIN_FUNC);
				if (pFunc)
				{
					info.pPlugin = pFunc(this);
					if (info.pPlugin)
					{
						mPlugins.emplace(name, info);
						return info.pPlugin;
					}
				}
			}
#endif
			return nullptr;
		}

		Plugin* PluginMgr::GetPlugin(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				return it->second.pPlugin;
			}
			return nullptr;
		}

		bool PluginMgr::Unload(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				if (it->second.pPlugin->Release())
				{
					mPlugins.erase(it);
				}
				return true;
			}
			return false;
		}

		void PluginMgr::UnloadAll()
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			for (auto it = mPlugins.begin();it != mPlugins.end();++it)
			{
				Unload(it->first);
			}
		}
	}
}