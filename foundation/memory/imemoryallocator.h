#pragma once
#include <vector>
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
		typedef void(*DestructListener)(const AllocationMap&);
		IMemoryAllocator(const DestructListener listener = nullptr)
			:m_destructListener(listener), m_allocatedSize(0){}
		IMemoryAllocator(const IMemoryAllocator&) = delete;
		IMemoryAllocator(IMemoryAllocator&&) = delete;
		size_t GetAllocatedSize()const { return m_allocatedSize; };
		virtual ~IMemoryAllocator() 
		{ 
			assert(m_allocationMap.empty());
			if (m_destructListener)
				m_destructListener(m_allocationMap); 
		}
		virtual void* Allocate(size_t size, const char* fileName, const char* className, size_t line) = 0;
		virtual void Deallocate(void*) = 0;
		virtual void* AllocateArray(size_t size, const char* fileName, const char* className, size_t line) = 0;
		virtual void DeallocateArray(void*) = 0;
		virtual const AllocationMap& GetAllocationMap()const = 0;
	protected:
		DestructListener m_destructListener;
		AllocationMap m_allocationMap;
		size_t m_allocatedSize;
	};
}