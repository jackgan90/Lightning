#pragma once
#include <unordered_map>
#include <cassert>
#include "foundationexportdef.h"

#define ALLOC(allocator, size, type) \
	static_cast<type*>((allocator)->Allocate((size), __FILE__, __FUNCTION__, __LINE__))

#define ALLOC_ARRAY(allocator, arraySize, type) ALLOC(allocator, (arraySize)*sizeof(type), type)

#define DEALLOC(allocator, pointer) \
	(allocator)->Deallocate((pointer))

namespace Lightning
{
	namespace Foundation
	{
		struct LIGHTNING_FOUNDATION_API MemoryInfo
		{
			void* address;
			size_t size;
#ifndef NDEBUG
			const char* className;
			const char* fileName;
			size_t line;
#endif
		};
		class LIGHTNING_FOUNDATION_API IMemoryAllocator
		{
		public:
			IMemoryAllocator() :m_allocatedSize(0), m_allocatedCount(0){}
			IMemoryAllocator(const IMemoryAllocator&) = delete;
			IMemoryAllocator& operator=(const IMemoryAllocator&) = delete;
			size_t GetAllocatedSize()const { return m_allocatedSize; };
			size_t GetAllocatedCount()const { return m_allocatedCount; }
			virtual ~IMemoryAllocator() 
			{ 
				assert(m_allocatedSize == 0 && m_allocatedCount == 0);
			}
			virtual void* Allocate(size_t size, const char* fileName, const char* className, size_t line) = 0;
			virtual void Deallocate(void*) = 0;
		protected:
			//align address pointed to by ptr to next alignment byte
			inline size_t MakeAlign(size_t ptr, size_t alignment)const {return (ptr & ~std::size_t(alignment - 1)) + alignment;}
			size_t m_allocatedSize;
			size_t m_allocatedCount;
		};
	}
}
