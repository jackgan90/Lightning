#pragma once
#include "allocatormanager.h"

#define DECLARE_LEAK_FREE_OBJECT_NAME(className) const char ObjectName[] = "#className";
#define DECLARE_CLASS_NAME(className) \
	struct className##ClassNameHolder \
	{ \
		static const char* GetClassName() \
		{ \
			return #className; \
		} \
	}; \

#define USE_CLASS_NAME(className) \
	className##ClassNameHolder

namespace memory
{
	//in fact className should be passed a const char* to indicate the class name
	//however c++ does not allow pass a c-style string literal as a template parameter
	//the walk around here is to declare the second template parameter as a type
	//we should pass an object which has a static method to get the name from
	template<typename AllocatorType, typename ClassName>
	class ILeakFreeObject
	{
	public:
		typedef AllocatorType allocator_type;
		typedef ClassName class_name_type;
		virtual ~ILeakFreeObject(){}

		void* operator new(size_t size)
		{ 
			auto pAllocator = GetAllocator();
			return pAllocator->Allocate(size, __FILE__, ClassName::GetClassName(), __LINE__);
		}

		void* operator new[](size_t size)
		{
			auto pAllocator = GetAllocator();
			return pAllocator->AllocateArray(size, __FILE__, ClassName::GetClassName(), __LINE__);
		}

		void operator delete(void* p)noexcept(true)
		{
			auto pAllocator = GetAllocator();
			return pAllocator->Deallocate(p);
		}

		void operator delete[](void* p)noexcept(true)
		{
			auto pAllocator = GetAllocator();
			return pAllocator->DeallocateArray(p);
		}
	private:
		static IMemoryAllocator* GetAllocator()
		{
			return AllocatorManager::Instance()->GetAllocator(AllocatorType::ClassID);
		}
	};
}