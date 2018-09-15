#include <cassert>
#include "basememoryallocator.h"

namespace Lightning
{
	namespace Foundation
	{
		BaseMemoryAllocator::BaseMemoryAllocator()
			:IMemoryAllocator()
		{

		}

		void* BaseMemoryAllocator::Allocate(size_t size, const char* fileName, const char* className, size_t line)
		{
			void* address = ::operator new(size);
			MemoryInfo memoryInfo{	address,
									size,
#ifndef NDEBUG
									className,
									fileName,
									line
#endif
			};

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
	}

}

