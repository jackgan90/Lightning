#pragma once

namespace Lightning
{
	namespace Plugins
	{
		class IRefObject
		{
		public:
			virtual ~IRefObject() = default;
			virtual void AddRef() = 0;
			virtual bool Release() = 0;
			virtual int GetRefCount()const = 0;
		};
	}
}