#include <gpubuffer.h>
#include <cassert>

namespace Lightning
{
	namespace Render
	{
		GPUBuffer::GPUBuffer(std::uint32_t bufferSize) :m_buffer(nullptr), m_bufferSize(bufferSize), m_usedSize(0)
		{

		}

		const std::uint8_t* GPUBuffer::GetBuffer()const
		{
			return m_buffer;
		}

		void GPUBuffer::SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)
		{
			assert(buffer && "buffer can't be nullptr!");
			assert(bufferSize <= m_bufferSize);
			m_buffer = buffer;
			m_usedSize = bufferSize;
		}

		std::uint32_t GPUBuffer::GetBufferSize()const
		{
			return m_bufferSize;
		}
	}
}
