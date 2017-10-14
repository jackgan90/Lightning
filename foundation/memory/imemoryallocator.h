#pragma once
#include <unordered_map>
#include <cassert>
#include "memoryexportdef.h"
#include "memorymacros.h"

#define ALLOC(allocator, size) \
	(allocator)->Allocate((size), __FILE__, __FUNCTION__, __LINE__)

#define DEALLOC(allocator, pointer) \
	(allocator)->Deallocate((pointer))

namespace LightningGE
{
	namespace Foundation
	{
		struct MEMORY_API MemoryInfo
		{
			const char* className;
			void* address;
			size_t size;
			const char* fileName;
			size_t line;
		};
		class MEMORY_API IMemoryAllocator
		{
		public:
			IMemoryAllocator() :m_allocatedSize(0), m_allocatedCount(0){}
			IMemoryAllocator(const IMemoryAllocator&) = delete;
			IMemoryAllocator& operator=(const IMemoryAllocator&) = delete;
			size_t GetAllocatedSize()const { return m_allocatedSize; };
			virtual ~IMemoryAllocator() 
			{ 
				assert(m_allocatedSize == 0 && m_allocatedCount == 0);
			}
			virtual void* Allocate(size_t size, const char* fileName, const char* className, size_t line) = 0;
			virtual void Deallocate(void*) = 0;
		protected:
			size_t m_allocatedSize;
			size_t m_allocatedCount;
		};
	}
}