#pragma once

#define GET_ALLOCATOR(allocatorType) \
	memory::##allocatorType* pAllocator = memory::AllocatorManager::Instance()->allocatorType##Instance()


#define MEMORY_ALLOCATOR_OPERATOR_OVERRIDE(classType, allocatorType) \
	public: \
		void* operator new(size_t size) \
		{ \
			GET_ALLOCATOR(allocatorType); \
			return pAllocator->Allocate(size, __FILE__, #classType, __LINE__); \
		} \
		void* operator new[](size_t size) \
		{ \
			GET_ALLOCATOR(allocatorType); \
			return pAllocator->AllocateArray(size, __FILE__, #classType, __LINE__); \
		} \
		void operator delete(void* p)noexcept(true) \
		{ \
			GET_ALLOCATOR(allocatorType); \
			return pAllocator->Deallocate(p); \
		} \
		void operator delete[](void* p)noexcept(true) \
		{ \
			GET_ALLOCATOR(allocatorType); \
			return pAllocator->DeallocateArray(p); \
		} \
		