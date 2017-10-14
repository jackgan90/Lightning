#include "poolallocator.h"

namespace LightningGE
{
	namespace Foundation
	{

		PoolAllocator::PoolAllocator(size_t size) :IMemoryAllocator(), m_size(size)
		{
			m_buffer = new char[size];
		}

		PoolAllocator::~PoolAllocator()
		{
			delete[] m_buffer;
			m_buffer = nullptr;
			m_size = 0;
		}

		void* PoolAllocator::Allocate(size_t size, const char* fileName, const char* className, size_t line)
		{
			return nullptr;
		}

		void PoolAllocator::Deallocate(void* p)
		{

		}
	}
}