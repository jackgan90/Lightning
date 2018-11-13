#include <gpubuffer.h>
#include <cassert>

namespace Lightning
{
	namespace Render
	{
		GPUBuffer::GPUBuffer(std::uint32_t bufferSize) :
			mBufferSize(bufferSize)
		{
		}

		std::uint32_t GPUBuffer::GetBufferSize()const
		{
			return mBufferSize;
		}
	}
}
