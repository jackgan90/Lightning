#include "PluginMgr.h"

namespace Lightning
{
	namespace Plugins
	{
		PluginMgr::PluginMgr()
		{

		}

		PluginMgr::~PluginMgr()
		{
		}

		void PluginMgr::Update()
		{

		}

		Plugin* PluginMgr::Load(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				it->second.plugin->AddRef();
				return it->second.plugin;
			}
			PluginInfo info;
#ifdef LIGHTNING_WIN32
			info.handle = ::LoadLibrary((name + Plugin::PluginExtension).c_str());
			if (info.handle)
			{
				GetPluginProc pGetProc = (GetPluginProc)::GetProcAddress(info.handle, GET_PLUGIN_PROC);
				if (pGetProc)
				{
					info.plugin = pGetProc(this);
					if (info.plugin)
					{
						info.plugin->SetName(name);
						mPlugins.emplace(name, info);
						return info.plugin;
					}
				}
				::FreeLibrary(info.handle);
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
				return it->second.plugin;
			}
			return nullptr;
		}

		bool PluginMgr::Unload(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				if (it->second.plugin->Release())
				{
#ifdef LIGHTNING_WIN32
					//it->second.ReleaseProc(it->second.pPlugin);
					::FreeLibrary(it->second.handle);
#endif
					mPlugins.erase(it);
				}
				return true;
			}
			return false;
		}
	}
}