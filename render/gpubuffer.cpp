#include <gpubuffer.h>
#include <cassert>

namespace LightningGE
{
	namespace Render
	{
		GPUBuffer::GPUBuffer() :m_buffer(nullptr), m_bufferSize(0)
		{

		}

		const std::uint8_t* GPUBuffer::GetBuffer()const
		{
			return m_buffer;
		}

		void GPUBuffer::SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)
		{
			assert(buffer && "buffer can't be nullptr!");
			m_buffer = buffer;
			m_bufferSize = bufferSize;
		}

		std::uint32_t GPUBuffer::GetBufferSize()const
		{
			return m_bufferSize;
		}
	}
}
