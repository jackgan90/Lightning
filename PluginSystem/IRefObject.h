#pragma once
#include "Portable.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IRefObject
		{
			virtual ~IRefObject() = default;
			virtual void AddRef() = 0;
			virtual bool Release() = 0;
			virtual int GetRefCount()const = 0;
		};
	}
}