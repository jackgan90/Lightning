#pragma once
#include <type_traits>
#include <string>
#include "IPlugin.h"

namespace Lightning
{
	namespace Plugins
	{
		//Interface for plugin management.
		class IPluginManager
		{
		public:
			virtual ~IPluginManager(){}
			virtual IPlugin* LoadPlugin(const std::string& pluginName) = 0;
			virtual IPlugin* GetPlugin(const std::string& pluginName) = 0;
			virtual bool UnloadPlugin(const std::string& pluginName) = 0;
			virtual void Update() = 0;
			//Adjust update order.Make plugin1's update happens before plugin2's
			virtual void MakePlugin1UpdateBeforePlugin2(IPlugin* plugin1, IPlugin* plugin2) = 0;
		};

		template<typename T>
		std::enable_if_t<std::is_base_of<IPlugin, T>::value, T*> LoadPlugin(IPluginManager* mgr,const std::string& pluginName)
		{
			return dynamic_cast<T*>(mgr->LoadPlugin(pluginName.c_str()));
		}
		
		template<typename T>
		std::enable_if_t<std::is_base_of<IPlugin, T>::value, T*> GetPlugin(IPluginManager* mgr,const std::string& pluginName)
		{
			return dynamic_cast<T*>(mgr->GetPlugin(pluginName.c_str()));
		}
	}
}