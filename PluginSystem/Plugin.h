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
		return NEW_REF_OBJ(Lightning::Plugins::##pluginImpl);\
	}\
}\

#define PLUGIN_GETNAME_METHOD const char* GetName()const override{ return mName.c_str(); }
#define PLUGIN_GETFULLNAME_METHOD const char* GetFullName()const override { return mFullName.c_str(); }

#define PLUGIN_SETNAME_METHOD \
void SetName(const char* name)override \
{ \
	mName = name; \
	mFullName = mName + std::string(PluginExtension); \
}

#define PLUGIN_SETUPDATEORDER_METHOD void SetUpdateOrder(int updateOrder)override { mUpdateOrder = updateOrder; }
#define PLUGIN_GETUPDATEORDER_METHOD int GetUpdateOrder()const override{ return mUpdateOrder; }
#define PLUGIN_PRIVATE_FIELDS \
std::string mName;\
std::string mFullName;\
int mUpdateOrder;

#define PLUGIN_OVERRIDE(ClassName) \
public: \
PLUGIN_GETNAME_METHOD \
PLUGIN_GETFULLNAME_METHOD \
PLUGIN_SETNAME_METHOD \
PLUGIN_SETUPDATEORDER_METHOD \
PLUGIN_GETUPDATEORDER_METHOD \
private: \
PLUGIN_PRIVATE_FIELDS \
REF_OBJECT_OVERRIDE(ClassName)

namespace Lightning
{
	namespace Plugins
	{
#ifdef LIGHTNING_WIN32
		constexpr char* PluginExtension = ".dll";
#endif
		extern "C" typedef IPlugin* (*GetPluginProc)(class IPluginManager*);
	}
}