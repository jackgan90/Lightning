#include <cassert>
#include "basememoryallocator.h"

namespace memory
{
	BaseMemoryAllocator::BaseMemoryAllocator(const DestructListener listener /*= nullptr*/)
		:IMemoryAllocator(listener)
	{

	}

	void* BaseMemoryAllocator::Allocate(size_t size, const char* fileName, const char* className, size_t line)
	{
		void* address = ::operator new(size);
		MemoryInfo memoryInfo{	className,
								address,
								size,
								fileName,
								line,
								ALLOCATION_TYPE_SINGLE };

		m_allocationMap[address] = memoryInfo;
		m_allocatedSize += size;
		return address;
	}

	void BaseMemoryAllocator::Deallocate(void* p)
	{
		auto it = m_allocationMap.find(p);
		assert(it != m_allocationMap.end() && "Try to deallocate a pointer that is not allocated by this allocator!");
		::operator delete(p);
		m_allocatedSize -= it->second.size;
		m_allocationMap.erase(it);
	}

	void* BaseMemoryAllocator::AllocateArray(size_t size, const char* fileName, const char* className, size_t line)
	{
		void* address = ::operator new[](size);
		MemoryInfo memoryInfo{	className,
								address,
								size,
								fileName,
								line,
								ALLOCATION_TYPE_ARRAY };

		m_allocationMap[address] = memoryInfo;
		m_allocatedSize += size;
		return address;
	}

	void BaseMemoryAllocator::DeallocateArray(void* p)
	{
		auto it = m_allocationMap.find(p);
		assert(it != m_allocationMap.end() && "Try to deallocate a pointer that is not allocated by this allocator!");
		::operator delete[](p);
		m_allocatedSize -= it->second.size;
		m_allocationMap.erase(it);
	}

	const AllocationMap& BaseMemoryAllocator::GetAllocationMap()const
	{
		return m_allocationMap;
	}

}

