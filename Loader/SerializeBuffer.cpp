#include "SerializeBuffer.h"

namespace Lightning
{
	namespace Loading
	{
		SerializeBuffer::SerializeBuffer(std::size_t size): mSize(size)
		{
			mBuffer = std::make_unique<char[]>(mSize + 1);
			mBuffer[mSize] = 0;
		}

		char* SerializeBuffer::GetBuffer()
		{
			return mBuffer.get();
		}

		std::size_t SerializeBuffer::GetBufferSize()const
		{
			return mSize;
		}
	}
}