#include <cassert>
#include <algorithm>
#include "vertexbuffer.h"
#include "rendererhelper.h"

namespace LightningGE
{
	namespace Renderer
	{
		VertexBuffer::VertexBuffer(const std::vector<VertexComponent>& components):
			m_components(components), m_buffer(nullptr), m_bufferSize(0), m_GPUBindSlot(0xffff)
			,m_vertexSize(0), m_vertexCount(0), m_vertexCountDirty(true), m_vertexSizeDirty(true)
		{

		}

		const VertexComponent& VertexBuffer::GetComponentInfo(size_t componentIndex)
		{
			assert(componentIndex < m_components.size());
			return m_components[componentIndex];
		}

		std::uint8_t VertexBuffer::GetComponentCount()
		{
			return static_cast<std::uint8_t>(m_components.size());
		}

		const std::uint8_t* VertexBuffer::GetBuffer()const
		{
			return m_buffer;
		}

		std::uint32_t VertexBuffer::GetBufferSize()
		{
			return m_bufferSize;
		}

		std::uint32_t VertexBuffer::GetVertexCount()
		{
			CalculateVertexCount();
			return m_vertexCount;
		}

		std::uint32_t VertexBuffer::GetVertexSize()
		{
			CalculateVertexSize();
			return m_vertexSize;
		}


		void VertexBuffer::SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)
		{
			assert(buffer && "buffer can't be nullptr!");
			m_buffer = buffer;
			m_bufferSize = bufferSize;
			m_vertexCountDirty = true;
			m_vertexSizeDirty = true;
		}


		void VertexBuffer::SetGPUBindSlot(std::uint16_t loc)
		{
			m_GPUBindSlot = loc;
		}

		std::uint16_t VertexBuffer::GetGPUBindSlot()const
		{
			return m_GPUBindSlot;
		}

		void VertexBuffer::CalculateVertexCount()
		{
			if (m_vertexCountDirty)
			{
				CalculateVertexSize();
				m_vertexCount = m_vertexSize > 0 ? m_bufferSize / m_vertexSize : 0;
				m_vertexCountDirty = false;
			}
		}

		void VertexBuffer::CalculateVertexSize()
		{
			if (m_vertexSizeDirty)
			{
				auto attribute = std::max_element(m_components.cbegin(), m_components.cend(),
					[](const VertexComponent& a0, const VertexComponent& a1) {return a0.offset < a1.offset; });
				m_vertexSize = attribute->offset + GetVertexFormatSize(attribute->format);
				m_vertexSizeDirty = false;
			}
		}
	}
}