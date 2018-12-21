#pragma once
#include "IRefObject.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IPlugin : public IRefObject
		{
			virtual const char* INTERFACECALL GetName()const = 0;
			virtual const char* INTERFACECALL GetFullName()const = 0;
			//The following methods are meant for IPluginManager to invoke,never invoke those methods directly
			virtual void INTERFACECALL Update() = 0;
			virtual void INTERFACECALL OnCreated(class IPluginManager*) = 0;
			virtual void INTERFACECALL SetName(const char* name) = 0;
			virtual void INTERFACECALL SetUpdateOrder(int order) = 0;
			virtual int INTERFACECALL GetUpdateOrder()const = 0;
		};
	}
}