#include <cassert>
#include <algorithm>
#include "vertexbuffer.h"
#include "rendererhelper.h"

namespace LightningGE
{
	namespace Render
	{
		VertexBuffer::VertexBuffer(const std::vector<VertexComponent>& components):
			m_components(components)
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
			GPUBuffer::SetBuffer(buffer, bufferSize);
			m_vertexCountDirty = true;
			m_vertexSizeDirty = true;
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