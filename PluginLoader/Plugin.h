#pragma once
#include <memory>
#include <string>
#include <atomic>
#ifdef LIGHTNING_WIN32
#include <Windows.h>
#endif

namespace Lightning
{
	namespace Plugins
	{
		class Plugin
		{
		public:
			std::string GetName()const { return mName; }
			//Decrease the reference count of this plugin,returns true if the plugin is deleted
			bool Release();
			void AddRef();
			std::string GetFullName()const;
		protected:
			friend class PluginMgr;
			Plugin(const std::string& name);
			Plugin(const Plugin&) = delete;
			Plugin& operator=(const Plugin&) = delete;
			//Load and Unload is only called by PluginMgr
			std::string mName;
			std::atomic<int> mRefCount;
#ifdef LIGHTNING_WIN32
			static constexpr char* PluginExtension = ".dll";
#endif
		};
		typedef Plugin* (*GetPluginProc)(class PluginMgr*);
	}
}