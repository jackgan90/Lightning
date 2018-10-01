#include <gpubuffer.h>
#include <cassert>

namespace Lightning
{
	namespace Render
	{
		GPUBuffer::GPUBuffer(std::uint32_t bufferSize) :
			mBufferSize(bufferSize), mDirty(true)
		{
			mBuffer = new std::uint8_t[bufferSize];
		}

		GPUBuffer::~GPUBuffer()
		{
			delete[] mBuffer;
		}

		std::uint8_t* GPUBuffer::Lock(std::size_t start, std::size_t size)
		{
			return mBuffer + start;
		}

		void GPUBuffer::Unlock(std::size_t start, std::size_t size)
		{
			mDirty = size > 0;
		}

		std::uint32_t GPUBuffer::GetBufferSize()const
		{
			return mBufferSize;
		}
	}
}
