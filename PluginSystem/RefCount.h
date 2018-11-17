#pragma once
#include <atomic>
#ifndef NDEBUG
#include <cassert>
#include <unordered_map>
#endif

#ifndef NDEBUG
#define NEW_REF_OBJ(Type, ...) new (__FILE__, #Type, __LINE__) Type(__VA_ARGS__)

class RefMonitor
{
public:
	static RefMonitor* Instance()
	{
		static RefMonitor instance;
		return &instance;
	}

	RefMonitor::~RefMonitor()
	{
		if (!mAllocations.empty())
		{
			assert(0);
		}
	}

	void* Allococate(std::size_t size, const char* file, const char* typeName, int line)
	{
		void* mem = std::malloc(size);
		Allocation allocation;
		allocation.file = file;
		allocation.typeName = typeName;
		allocation.line = line;
		allocation.size = size;
		mAllocations[mem] = allocation;
		return mem;
	}

	void Deallocate(void* p)
	{
		std::free(p);
		mAllocations.erase(p);
	}
private:
	struct Allocation
	{
		std::size_t size;
		const char* file;
		const char* typeName;
		int line;
	};
	std::unordered_map<void*, Allocation> mAllocations;
};
#else
#define NEW_REF_OBJ(Type, ...) new Type(__VA_ARGS__)
#endif

namespace Lightning
{
	namespace Plugins
	{
		class RefCount
		{
		public:
			virtual ~RefCount() = default;
			RefCount():mRefCount(1)
			{
			}
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
#ifndef NDEBUG
			void* operator new(std::size_t size, const char* file, const char* typeName, int line)
			{
				return RefMonitor::Instance()->Allococate(size, file, typeName, line);
			}

			void operator delete(void *p)
			{
				RefMonitor::Instance()->Deallocate(p);
			}

			void operator delete(void *p, const char* file, const char* typeName, int line) 
			{
				RefMonitor::Instance()->Deallocate(p);
			}
#endif
		private:
			std::atomic<int> mRefCount;
		};
	}
}