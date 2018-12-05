#include "SerializeBuffer.h"

namespace Lightning
{
	namespace Loading
	{
		SerializeBuffer::SerializeBuffer(std::size_t size): mSize(size)
		{
			mBuffer = new char[mSize + 1];
			mBuffer[mSize] = 0;
		}

		SerializeBuffer::~SerializeBuffer()
		{
			delete[] mBuffer;
		}

		char* SerializeBuffer::GetBuffer()
		{
			return mBuffer;
		}

		std::size_t SerializeBuffer::GetBufferSize()const
		{
			return mSize;
		}
	}
}