#include <cassert>
#include <algorithm>
#include "vertexbuffer.h"
#include "rendererhelper.h"

namespace Lightning
{
	namespace Render
	{
		VertexBuffer::VertexBuffer(uint32_t bufferSize, const VertexComponent *components, std::uint8_t componentCount):
			IVertexBuffer(bufferSize), m_components(nullptr), m_componentCount(componentCount)
			,m_vertexSize(0), m_vertexCount(0)
		{
			if (m_componentCount)
			{
				m_components = new VertexComponent[m_componentCount];
				std::memcpy(m_components, components, m_componentCount * sizeof(VertexComponent));
			}
			CalculateVertexSize();
		}

		VertexBuffer::~VertexBuffer()
		{
			if (m_components)
			{
				delete[] m_components;
			}
		}

		const VertexComponent& VertexBuffer::GetComponentInfo(size_t componentIndex)
		{
			assert(componentIndex < m_componentCount);
			return m_components[componentIndex];
		}

		std::uint8_t VertexBuffer::GetComponentCount()
		{
			return m_componentCount;
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
			for (std::size_t i = 0;i < m_componentCount;++i)
			{
				m_vertexSize += GetVertexFormatSize(m_components[i].format);
			}
		}
	}
}