#pragma once
#include "IRefObject.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IPlugin : public IRefObject
		{
			virtual const char* GetName()const = 0;
			virtual const char* GetFullName()const = 0;
			//The following methods are meant for IPluginManager to invoke,never invoke those methods directly
			virtual void Update() = 0;
			virtual void OnCreated(class IPluginManager*) = 0;
			virtual void SetName(const char* name) = 0;
			virtual void SetUpdateOrder(int order) = 0;
			virtual int GetUpdateOrder()const = 0;
		};
	}
}