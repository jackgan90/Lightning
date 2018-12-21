#include <algorithm>
#include <cassert>
#include "PluginManager.h"
#include "Plugin.h"

namespace Lightning
{
	namespace Plugins
	{
		PluginManager::PluginManager():mNeedSyncPlugins(false), mPluginUpdatePriority(1024)
		{

		}

		PluginManager::~PluginManager()
		{
			SynchronizeTables();
		}

		void PluginManager::MakePlugin1UpdateBeforePlugin2(IPlugin* plugin1, IPlugin* plugin2)
		{
			assert(plugin1 && plugin2 && "Both plugin1 and plugin2 must be valid!");
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			plugin1->SetUpdateOrder(plugin2->GetUpdateOrder() - 1);
			mNeedSyncPlugins = true;
		}

		void PluginManager::Update()
		{
			for (auto& pluginInfo : mPluginsToUpdate)
			{
				pluginInfo.plugin->Update();
			}

			SynchronizeTables();
		}

		void PluginManager::SynchronizeTables()
		{
			if (mNeedSyncPlugins)
			{
				std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
				while (!mOperationQueue.empty())
				{
					auto operation = mOperationQueue.front();
					mOperationQueue.pop_front();
					switch (operation.type)
					{
					case OperationType::Load:
						mPlugins[operation.name] = mPendingAddPlugins[operation.name];
						break;
					case OperationType::Unload:
						UnloadPlugin(mPlugins, operation.name);
						break;
					default:
						break;
					}
				}
				mPendingAddPlugins.clear();
				mPluginsToUpdate.clear();
				for (auto it = mPlugins.begin(); it != mPlugins.end();++it)
				{
					mPluginsToUpdate.push_back(it->second);
				}
				std::sort(mPluginsToUpdate.begin(), mPluginsToUpdate.end(), 
					[this](const PluginInfo& info1, const PluginInfo& info2) {
					return ComparePluginUpdateOrder(info1, info2);
				});
				mNeedSyncPlugins = false;
			}
		}

		bool PluginManager::ComparePluginUpdateOrder(const PluginInfo& info1, const PluginInfo& info2)
		{
			return info1.plugin->GetUpdateOrder() < info2.plugin->GetUpdateOrder();
		}

		IPlugin* PluginManager::LoadPlugin(const char* szName)
		{
			std::string name(szName);
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);

			auto plugin = LookUpPlugin(mPendingAddPlugins, name, true);
			if (plugin)
				return plugin;

			plugin = LookUpPlugin(mPlugins, name, true);
			if (plugin)
				return plugin;

			PluginInfo info;
#ifdef LIGHTNING_WIN32
			info.handle = ::LoadLibrary((name + PluginExtension).c_str());
			if (info.handle)
			{
				GetPluginProc pGetProc = (GetPluginProc)::GetProcAddress(info.handle, GET_PLUGIN_PROC);
				if (pGetProc)
				{
					info.plugin = pGetProc(this);
					if (info.plugin)
					{
						info.plugin->SetName(name.c_str());
						info.plugin->SetUpdateOrder(mPluginUpdatePriority.fetch_add(1, std::memory_order_relaxed));
						info.plugin->OnCreated(this);
						//mPlugins.emplace(name, info);
						mPendingAddPlugins.emplace(name, info);
						Operation operation;
						operation.info = info;
						operation.name = name;
						operation.type = OperationType::Load;
						mOperationQueue.push_back(operation);
						//Needs to be the last statement.So that main thread will get data from operation queue.
						mNeedSyncPlugins = true;
						return info.plugin;
					}
				}
				::FreeLibrary(info.handle);
			}
#endif
			return nullptr;
		}

		IPlugin* PluginManager::LookUpPlugin(PluginTable& table, const std::string& name, bool addRef)
		{
			if (table.empty())
				return nullptr;
			auto it = table.find(name);
			if (it != table.end())
			{
				if (addRef)
					it->second.plugin->AddRef();
				return it->second.plugin;
			}
			return nullptr;
		}

		IPlugin* PluginManager::GetPlugin(const char* szName)
		{
			std::string name(szName);
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);

			auto plugin = LookUpPlugin(mPendingAddPlugins, name, false);
			if (plugin)
				return plugin;

			plugin = LookUpPlugin(mPlugins, name, false);
			if (plugin)
				return plugin;

			return nullptr;
		}

		bool PluginManager::UnloadPlugin(PluginTable& table, const std::string& name)
		{
			auto it = table.find(name);
			if (it != table.end())
			{
				if (it->second.plugin->Release())
				{
#ifdef LIGHTNING_WIN32
					::FreeLibrary(it->second.handle);
#endif
					table.erase(it);
				}
				return true;
			}
			return false;
		}

		bool PluginManager::UnloadPlugin(const char* szName)
		{
			std::string name(szName);
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			auto res = UnloadPlugin(mPendingAddPlugins, name);
			if (res)
				return true;

			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				Operation operation;
				operation.info = it->second;
				operation.name = name;
				operation.type = OperationType::Unload;
				mOperationQueue.push_back(operation);
				mNeedSyncPlugins = true;
				return true;
			}
			return false;
		}
	}
}