#pragma once
#include <memory>
#include <cassert>
#include <string>
#include <atomic>
#include "RefObject.h"
#include "IPlugin.h"

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
#ifdef LIGHTNING_WIN32
		constexpr char* PluginExtension = ".dll";
#endif
		class Plugin : public IPlugin
		{
		public:
			const char* GetName()const override{ return mName.c_str(); }
			//Decrease the reference count of this plugin,returns true if the plugin is deleted
			const char* GetFullName()const override
			{
				return mFullName.c_str();
			}
			void SetName(const char* name)override
			{
				mName = name;
				mFullName = name + std::string(PluginExtension);
			}
			void SetUpdateOrder(int updateOrder)override
			{
				mUpdateOrder = updateOrder;
			}
			int GetUpdateOrder()const override{ return mUpdateOrder; }
		protected:
			Plugin() : mName("")
			{
			}
		private:
			std::string mName;
			std::string mFullName;
			int mUpdateOrder;
			REF_OBJECT_OVERRIDE(Plugin)
		};
		typedef Plugin* (*GetPluginProc)(class IPluginManager*);
	}
}