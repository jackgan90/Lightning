#pragma once
#include <string>

namespace Lightning
{
	namespace Plugins
	{
		struct IPlugin
		{
			virtual ~IPlugin() = default;
			virtual std::string GetName()const = 0;
			virtual std::string GetFullName()const = 0;
			//The following methods are meant for IPluginManager to invoke,never invoke those methods directly
			virtual void Tick() = 0;
			virtual void OnCreated(class IPluginManager*) = 0;
			virtual void SetName(const std::string& name) = 0;
			virtual void SetTickOrder(int order) = 0;
			virtual int GetTickOrder()const = 0;
		};
	}
}