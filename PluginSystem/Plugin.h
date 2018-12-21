#pragma once
#include <memory>
#include <cassert>
#include <string>
#include <atomic>
#include "RefObject.h"

#ifdef LIGHTNING_WIN32
#define LIGHTNING_PLUGIN_DLL_EXPORT __declspec(dllexport)
#else
#define LIGHTNING_PLUGIN_DLL_EXPORT
#endif

#define LIGHTNING_PLUGIN_IMPL(pluginImpl)\
namespace Lightning\
{\
	namespace Plugins\
	{\
		IPluginManager* gPluginMgr;\
	}\
}\
extern "C"\
{\
	LIGHTNING_PLUGIN_DLL_EXPORT Lightning::Plugins::Plugin* GetPlugin(Lightning::Plugins::IPluginManager* mgr)\
	{\
		Lightning::Plugins::gPluginMgr = mgr;\
		return NEW_REF_OBJ(Lightning::Plugins::##pluginImpl);\
	}\
}\

namespace Lightning
{
	namespace Plugins
	{
		class Plugin : public IRefObject
		{
		public:
			std::string GetName()const { return mName; }
			//Decrease the reference count of this plugin,returns true if the plugin is deleted
			std::string GetFullName()const
			{
				return mName + PluginExtension;
			}
			//Update is called by IPluginMgr and tick at a regular speed rate.
			virtual void Update() = 0;
		protected:
			friend class PluginManager;
			Plugin() : mName("")
			{
			}
			virtual void OnCreated(class IPluginManager*) = 0;
			//Load and Unload is only called by PluginMgr
		private:
			void SetName(const std::string& name)
			{
				mName = name;
			}
			void SetUpdateOrder(int updateOrder)
			{
				mUpdateOrder = updateOrder;
			}
			int GetUpdateOrder()const { return mUpdateOrder; }
			std::string mName;
			int mUpdateOrder;
#ifdef LIGHTNING_WIN32
			static constexpr char* PluginExtension = ".dll";
#endif
			REF_OBJECT_OVERRIDE(Plugin)
		};
		typedef Plugin* (*GetPluginProc)(class IPluginManager*);
	}
}