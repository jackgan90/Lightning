#pragma once
#include <string>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <deque>
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
			PluginMgr();
			~PluginMgr();
			bool Unload(PluginTable& table, const std::string& name);
			Plugin* LookUpPlugin(PluginTable& table, const std::string& pluginName, bool addRef);
			void SynchronizeTables();
			PluginTable mPlugins;
			PluginTable mPendingAddPlugins;
			std::deque<Operation> mOperationQueue;
			std::atomic<bool> mNeedSyncPlugins;
			std::recursive_mutex mPluginsMutex;
			static constexpr char* GET_PLUGIN_PROC = "GetPlugin";
		};
	}
}