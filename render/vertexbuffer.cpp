#include <cassert>
#include <algorithm>
#include "vertexbuffer.h"
#include "rendererhelper.h"

namespace Lightning
{
	namespace Render
	{
		VertexBuffer::VertexBuffer(uint32_t bufferSize, const std::vector<VertexComponent>& components):
			IVertexBuffer(bufferSize),m_components(components)
			,m_vertexSize(0), m_vertexCount(0)
		{
			CalculateVertexSize();
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

		std::uint32_t VertexBuffer::GetVertexCount()const 
		{
			return m_vertexCount;
		}

		std::uint32_t VertexBuffer::GetVertexSize()const
		{
			return m_vertexSize;
		}


		void VertexBuffer::SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)
		{
			IVertexBuffer::SetBuffer(buffer, bufferSize);
			m_vertexCount = m_usedSize / m_vertexSize;
		}


		void VertexBuffer::CalculateVertexSize()
		{
			m_vertexSize = 0;
			for (const auto& component : m_components)
			{
				m_vertexSize += GetVertexFormatSize(component.format);
			}
		}
	}
}