#pragma once
#include "imemoryallocator.h"

namespace LightningGE
{
	namespace Foundation
	{
		class MEMORY_API PoolAllocator : public IMemoryAllocator
		{
		public:	
			PoolAllocator(const size_t size);
			~PoolAllocator()override;
			void* Allocate(size_t size, const char* fileName, const char* className, size_t line)override;
			void Deallocate(void*)override;
		private:
			size_t m_size;
			char* m_buffer;
		};
	}
}