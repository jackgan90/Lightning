#pragma once
#include <atomic>

namespace Lightning
{
	namespace Plugins
	{
		class RefCount
		{
		public:
			virtual ~RefCount() = default;
			RefCount():mRefCount(1){}
			//Although AddRef and Release are declared virtual,subclasses should not try to override them
			//This is just a trick to make compiler generate late binding code.We have to invoke delete in the
			//dll which creates the object.
			virtual bool Release()
			{
				auto oldRefCountBase = mRefCount.fetch_sub(1, std::memory_order_relaxed);
				if (oldRefCountBase == 1)
				{
					delete this;
					return true;
				}
				return false;
			}
			virtual void AddRef()
			{
				mRefCount.fetch_add(1, std::memory_order_relaxed);
			}
			//This method is only for debug purpose,logic should not rely on the value returns by this method
			virtual int GetRefCount()const { return mRefCount; }
		private:
			std::atomic<int> mRefCount;
		};
	}
}