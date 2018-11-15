#pragma once
#include <type_traits>
#include "Plugin.h"

namespace Lightning
{
	namespace Plugins
	{
		class IPluginMgr
		{
		public:
			virtual ~IPluginMgr(){}
			virtual Plugin* Load(const std::string& pluginName) = 0;
			virtual Plugin* GetPlugin(const std::string& pluginName) = 0;
			virtual bool Unload(const std::string& pluginName) = 0;
			virtual void UnloadAll() = 0;
			template<typename T>
			std::enable_if_t<std::is_base_of<Plugin, T>::value, T*> Load(const std::string& pluginName)
			{
				return static_cast<T*>(Load(pluginName));
			}
			
			template<typename T>
			std::enable_if_t<std::is_base_of<Plugin, T>::value, T*> GetPlugin(const std::string& pluginName)
			{
				return static_cast<T*>(GetPlugin(pluginName));
			}
		};
	}
}