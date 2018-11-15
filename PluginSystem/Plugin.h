#pragma once
#include <memory>
#include <cassert>
#include <string>
#include <atomic>

namespace Lightning
{
	namespace Plugins
	{
		typedef void(*ReleasePluginProc)(class Plugin*);
		typedef Plugin* (*GetPluginProc)(const char* name, class IPluginMgr*);

		class Plugin
		{
		public:
			std::string GetName()const { return mName; }
			//Decrease the reference count of this plugin,returns true if the plugin is deleted
			std::string GetFullName()const
			{
				return mName + PluginExtension;
			}
			virtual ~Plugin(){}
		protected:
			friend class PluginMgr;
			Plugin(const char* name) : mName(name), mRefCount(1)
			{
			}
			Plugin(const Plugin&) = delete;
			Plugin& operator=(const Plugin&) = delete;
			virtual void Finalize() = 0;
			//Load and Unload is only called by PluginMgr
			std::string mName;
			std::atomic<int> mRefCount;
		private:
			bool Release()
			{
				auto oldRefCount = mRefCount.fetch_sub(1, std::memory_order_relaxed);
				if (oldRefCount == 1)
				{
					Finalize();
					return true;
				}
				return false;
			}
			void AddRef()
			{
				mRefCount.fetch_add(1, std::memory_order_relaxed);
			}
#ifdef LIGHTNING_WIN32
			static constexpr char* PluginExtension = ".dll";
#endif
		};
	}
}