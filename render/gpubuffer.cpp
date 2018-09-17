#include <gpubuffer.h>
#include <cassert>

namespace Lightning
{
	namespace Render
	{
		GPUBuffer::GPUBuffer(std::uint32_t bufferSize) :mBuffer(nullptr), mBufferSize(bufferSize), mUsedSize(0)
		{

		}

		const std::uint8_t* GPUBuffer::GetBuffer()const
		{
			return mBuffer;
		}

		void GPUBuffer::SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)
		{
			assert(buffer && "buffer can't be nullptr!");
			assert(bufferSize <= mBufferSize);
			mBuffer = buffer;
			mUsedSize = bufferSize;
		}

		std::uint32_t GPUBuffer::GetBufferSize()const
		{
			return mBufferSize;
		}
	}
}
