#pragma once
#include <cstdint>
#include "Container.h"

namespace Lightning
{
	namespace Foundation
	{
		//Allocate memory for objects that won't release(for example vertex formats)
		template<typename T, int BufferObjects=100>
		class MemoryPool
		{
		public:
			T* Get()
			{
				Buffer *buffer{ nullptr };
				if (mBuffers.empty() || mBuffers.back().current + sizeof(T) > BUFFER_SIZE)
				{
					mBuffers.emplace_back();
				}
				buffer = &mBuffers.back();
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
			Container::Vector<Buffer> mBuffers;
			static constexpr const std::size_t BUFFER_SIZE = sizeof(T) * BufferObjects;
		};
	}
}