#pragma once
#include <unordered_map>
#include <cassert>
#include "memoryexportdef.h"
#include "memorymacros.h"

namespace memory
{
	enum AllocationType
	{
		ALLOCATION_TYPE_SINGLE,	
		ALLOCATION_TYPE_ARRAY
	};

	struct MEMORY_API MemoryInfo
	{
		const char* className;
		void* address;
		size_t size;
		const char* fileName;
		size_t line;
		AllocationType allocType;
	};
	typedef std::unordered_map<void*, MemoryInfo> AllocationMap;
	class MEMORY_API IMemoryAllocator
	{
	public:
		IMemoryAllocator() :m_allocatedSize(0){}
		IMemoryAllocator(const IMemoryAllocator&) = delete;
		IMemoryAllocator& operator=(const IMemoryAllocator&) = delete;
		size_t GetAllocatedSize()const { return m_allocatedSize; };
		virtual ~IMemoryAllocator() 
		{ 
			assert(m_allocationMap.empty());
		}
		virtual void* Allocate(size_t size, const char* fileName, const char* className, size_t line) = 0;
		virtual void Deallocate(void*) = 0;
		virtual void* AllocateArray(size_t size, const char* fileName, const char* className, size_t line) = 0;
		virtual void DeallocateArray(void*) = 0;
	protected:
		AllocationMap m_allocationMap;
		size_t m_allocatedSize;
	};
}