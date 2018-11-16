#pragma once
#include <memory>
#include <cassert>
#include <string>
#include <atomic>

#ifdef LIGHTNING_WIN32
#define LIGHTNING_PLUGIN_DLL_EXPORT __declspec(dllexport)
#else
#define LIGHTNING_PLUGIN_DLL_EXPORT
#endif

#define LIGHTNING_PLUGIN_INTERFACE(pluginImpl)\
namespace Lightning\
{\
	namespace Plugins\
	{\
		IPluginMgr* gPluginMgr;\
	}\
}\
extern "C"\
{\
	LIGHTNING_PLUGIN_DLL_EXPORT Lightning::Plugins::Plugin* GetPlugin(Lightning::Plugins::IPluginMgr* mgr)\
	{\
		Lightning::Plugins::gPluginMgr = mgr;\
		return new Lightning::Plugins::##pluginImpl();\
	}\
}\

namespace Lightning
{
	namespace Plugins
	{
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
			friend class IPluginMgr;
			friend class PluginMgr;
			Plugin() : mName(""), mRefCount(1)
			{
			}
			Plugin(const Plugin&) = delete;
			Plugin& operator=(const Plugin&) = delete;
			//Load and Unload is only called by PluginMgr
			std::string mName;
			int GetRefCount()const { return mRefCount; }
		private:
			virtual bool Release()
			{
				auto oldRefCount = mRefCount.fetch_sub(1, std::memory_order_relaxed);
				if (oldRefCount == 1)
				{
					delete this;
					return true;
				}
				return false;
			}
			virtual void AddRef()
			{
				mRefCount.fetch_add(1, std::memory_order_relaxed);
			}
			void SetName(const std::string& name)
			{
				mName = name;
			}
			std::atomic<int> mRefCount;
#ifdef LIGHTNING_WIN32
			static constexpr char* PluginExtension = ".dll";
#endif
		};
		typedef Plugin* (*GetPluginProc)(class IPluginMgr*);
	}
}