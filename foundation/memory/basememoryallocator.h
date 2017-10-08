#pragma once
#include <unordered_map>
#include "allocatorid.h"
#include "imemoryallocator.h"

namespace memory
{
	//just a simple wrapper of system allocator,mainly design for hook allocation process
	class MEMORY_API BaseMemoryAllocator : public IMemoryAllocator
	{
	public:
		static const AllocatorID ClassID = AllocatorID::ALLOCATOR_ID_BASE;
		BaseMemoryAllocator(const DestructListener listener = nullptr);
		void* Allocate(size_t size, const char* fileName, const char* className, size_t line)override;
		void Deallocate(void*)override;
		void* AllocateArray(size_t size, const char* fileName, const char* className, size_t line)override;
		void DeallocateArray(void*)override;
		const AllocationMap& GetAllocationMap()const override;
	};
}