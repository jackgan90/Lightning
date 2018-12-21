#pragma once
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <deque>
#include "IPluginManager.h"
#ifdef LIGHTNING_WIN32
#include <Windows.h>
#endif


namespace Lightning
{
	class Engine;
	namespace Plugins
	{
		//An instance of a PluginMgr can only be created by Engine.
		class PluginManager : public IPluginManager
		{
		public:
			IPlugin* LoadPlugin(const char* pluginName)override;
			IPlugin* GetPlugin(const char* pluginName)override;
			bool UnloadPlugin(const char* pluginName)override;
			void Update()override;
			void MakePlugin1UpdateBeforePlugin2(IPlugin* plugin1, IPlugin* plugin2)override;
		private:
			friend class Engine;
			struct PluginInfo
			{
				IPlugin* plugin;
#ifdef LIGHTNING_WIN32
				HMODULE handle;
#endif
			};
			enum class OperationType
			{
				Load,
				Unload,
			};
			struct Operation
			{
				PluginInfo info;
				OperationType type;
				std::string name;
			};
			using PluginTable = std::unordered_map<std::string, PluginInfo>;
			using PluginList = std::vector<PluginInfo>;
			PluginManager();
			~PluginManager();
			bool UnloadPlugin(PluginTable& table, const std::string& name);
			IPlugin* LookUpPlugin(PluginTable& table, const std::string& pluginName, bool addRef);
			void SynchronizeTables();
			bool ComparePluginUpdateOrder(const PluginInfo&, const PluginInfo&);
			PluginTable mPlugins;
			PluginTable mPendingAddPlugins;
			PluginList mPluginsToUpdate;
			std::deque<Operation> mOperationQueue;
			std::atomic<bool> mNeedSyncPlugins;
			std::atomic<int> mPluginUpdatePriority;
			std::recursive_mutex mPluginsMutex;
			static constexpr char* GET_PLUGIN_PROC = "GetPlugin";
		};
	}
}