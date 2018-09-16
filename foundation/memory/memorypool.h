#pragma once
#include <vector>
#include <cstdint>
#include "singleton.h"

namespace Lightning
{
	namespace Foundation
	{
		//Allocate memory for objects that won't release(for example vertex formats)
		template<typename T>
		class MemoryPool : public Singleton<MemoryPool>
		{
		public:
			T* Get()
			{
				Buffer *buffer{ nullptr };
				if (m_buffers.empty() || m_buffers.back().current + sizeof(T) > BUFFER_SIZE)
				{
					m_buffers.emplace_back();
				}
				buffer = &m_buffers.back();
				T* ret = static_cast<T*>(buffer->ptr);
				buffer->current += sizeof(T);
				return ret;
			}
		private:
			struct Buffer
			{
				Buffer() : current(0)
				{
					ptr = new char[BUFFER_SIZE];
				}
				~Buffer()
				{
					delete[] ptr;
				}
				char* ptr;
				std::size_t current;
			};
			std::vector<Buffer> m_buffers;
			static constexpr const std::size_t BUFFER_SIZE = sizeof(T) * 100;
		};
	}
}