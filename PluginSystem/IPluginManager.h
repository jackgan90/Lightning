#pragma once
#include <type_traits>
#include "Plugin.h"

namespace Lightning
{
	namespace Plugins
	{
		//Interface for plugin management.
		class IPluginManager
		{
		public:
			virtual ~IPluginManager(){}
			virtual Plugin* LoadPlugin(const std::string& pluginName) = 0;
			virtual Plugin* GetPlugin(const std::string& pluginName) = 0;
			virtual bool UnloadPlugin(const std::string& pluginName) = 0;
			virtual void Update() = 0;
			//Adjust update order.Make plugin1's update happens before plugin2's
			virtual void MakePlugin1UpdateBeforePlugin2(Plugin* plugin1, Plugin* plugin2) = 0;
			template<typename T>
			std::enable_if_t<std::is_base_of<Plugin, T>::value, T*> LoadPlugin(const std::string& pluginName)
			{
				return static_cast<T*>(LoadPlugin(pluginName));
			}
			
			template<typename T>
			std::enable_if_t<std::is_base_of<Plugin, T>::value, T*> GetPlugin(const std::string& pluginName)
			{
				return static_cast<T*>(GetPlugin(pluginName));
			}
		};
	}
}