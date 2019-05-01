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
	LIGHTNING_PLUGIN_DLL_EXPORT Lightning::Plugins::IPlugin* GetPlugin(Lightning::Plugins::IPluginManager* mgr)\
	{\
		Lightning::Plugins::gPluginMgr = mgr;\
		return new Lightning::Plugins::##pluginImpl;\
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
			std::string GetName()const override{ return mName; }
			std::string GetFullName()const override { return mFullName; }
			void SetName(const std::string& name)override 
			{ 
				mName = name; 
				mFullName = mName + std::string(PluginExtension); 
			}
			void SetUpdateOrder(int updateOrder)override { mUpdateOrder = updateOrder; }
			int GetUpdateOrder()const override{ return mUpdateOrder; }
		protected:
			std::string mName;
			std::string mFullName;
			int mUpdateOrder;
		};
		extern "C" typedef IPlugin* (*GetPluginProc)(class IPluginManager*);
	}
}