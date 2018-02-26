#pragma once
#include <unordered_map>
#include "allocatorid.h"
#include "imemoryallocator.h"

namespace LightningGE
{
	namespace Foundation
	{
		//just a simple wrapper of system allocator,mainly design for hook allocation process
		class LIGHTNINGGE_FOUNDATION_API BaseMemoryAllocator : public IMemoryAllocator
		{
		public:
			static const AllocatorID ClassID = AllocatorID::BASE;
			BaseMemoryAllocator();
			void* Allocate(size_t size, const char* fileName, const char* className, size_t line)override;
			void Deallocate(void*)override;
		private:
			using AllocationMap = std::unordered_map<void*, MemoryInfo>;
			AllocationMap m_allocationMap;

		};
	}
}