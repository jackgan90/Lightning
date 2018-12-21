#pragma once
#include "Portable.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IRefObject
		{
			virtual INTERFACECALL ~IRefObject() = default;
			virtual void INTERFACECALL AddRef() = 0;
			virtual bool INTERFACECALL Release() = 0;
			virtual int INTERFACECALL GetRefCount()const = 0;
		};
	}
}