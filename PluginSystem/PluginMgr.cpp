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
			UnloadAllImpl();
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
			info.handle = ::LoadLibrary((name + Plugin::PluginExtension).c_str());
			if (info.handle)
			{
				GetPluginProc pGetProc = (GetPluginProc)::GetProcAddress(info.handle, GET_PLUGIN_PROC);
				ReleasePluginProc pReleaseProc = (ReleasePluginProc)::GetProcAddress(info.handle, RELEASE_PLUGIN_PROC);
				if (pGetProc && pReleaseProc)
				{
					info.ReleaseProc = pReleaseProc;
					info.pPlugin = pGetProc(name.c_str(), this);
					if (info.pPlugin)
					{
						mPlugins.emplace(name, info);
						return info.pPlugin;
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
				return it->second.pPlugin;
			}
			return nullptr;
		}

		bool PluginMgr::Unload(const std::string& name)
		{
			auto result = UnloadImpl(name);
			return std::get<0>(result);
		}

		void PluginMgr::UnloadAll()
		{
			UnloadAllImpl();
		}

		std::tuple<bool, PluginMgr::PluginTable::iterator> PluginMgr::UnloadImpl(const std::string& name)
		{
			std::tuple<bool, PluginMgr::PluginTable::iterator> result;
			std::get<0>(result) = false;
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			std::get<1>(result) = mPlugins.end();
			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				if (it->second.pPlugin->Release())
				{
#ifdef LIGHTNING_WIN32
					it->second.ReleaseProc(it->second.pPlugin);
					::FreeLibrary(it->second.handle);
#endif
					std::get<1>(result) = mPlugins.erase(it);
				}
				std::get<0>(result) = true;
			}
			return result;
		}

		void PluginMgr::UnloadAllImpl()
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			for (auto it = mPlugins.begin();it != mPlugins.end();)
			{
				auto result = UnloadImpl(it->first);
				bool succeed = std::get<0>(result);
				auto& nit = std::get<1>(result);
				if (succeed)
				{
					if (nit != mPlugins.end())
						it = nit;
					else
						break;
				}
				else
					++it;
			}
		}
	}
}