#include <cassert>
#include <algorithm>
#include "vertexbuffer.h"
#include "rendererhelper.h"

namespace LightningGE
{
	namespace Renderer
	{
		VertexBuffer::VertexBuffer():m_buffer(nullptr), m_bufferSize(0), m_bindingLocation(-1)
			,m_vertexSize(0), m_vertexCount(0), m_vertexCountDirty(true), m_vertexSizeDirty(true)
		{

		}

		const VertexAttribute& VertexBuffer::GetAttributeInfo(size_t attributeIndex)
		{
			assert(attributeIndex < m_attributes.size());
			return m_attributes[attributeIndex];
		}

		unsigned int VertexBuffer::GetAttributeCount()
		{
			return m_attributes.size();
		}

		const unsigned char* VertexBuffer::GetBuffer()
		{
			return m_buffer;
		}

		unsigned long VertexBuffer::GetBufferSize()
		{
			return m_bufferSize;
		}

		unsigned long VertexBuffer::GetVertexCount()
		{
			CalculateVertexCount();
			return m_vertexCount;
		}

		unsigned int VertexBuffer::GetVertexSize()
		{
			CalculateVertexSize();
			return m_vertexSize;
		}


		void VertexBuffer::SetBuffer(const std::vector<VertexAttribute>& attributes, unsigned char* buffer, unsigned int bufferSize)
		{
			m_attributes = attributes;
			m_buffer = buffer;
			m_bufferSize = bufferSize;
			m_vertexCountDirty = true;
			m_vertexSizeDirty = true;
		}


		void VertexBuffer::SetBindingLocation(int loc)
		{
			m_bindingLocation = loc;
		}

		int VertexBuffer::GetBindingLocation()const
		{
			return m_bindingLocation;
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
				auto attribute = std::max_element(m_attributes.begin(), m_attributes.end(),
					[&](const VertexAttribute& a0, const VertexAttribute& a1) {return GetVertexFormatSize(a0.format) < GetVertexFormatSize(a1.format); });
				m_vertexSize = attribute->offset + GetVertexFormatSize(attribute->format);
				m_vertexSizeDirty = false;
			}
		}
	}
}