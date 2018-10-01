#include <cassert>
#include <algorithm>
#include "vertexbuffer.h"
#include "rendererhelper.h"

namespace Lightning
{
	namespace Render
	{
		VertexBuffer::VertexBuffer(uint32_t bufferSize, const VertexDescriptor& descriptor):
			IVertexBuffer(bufferSize), mVertexDescriptor(descriptor)
			,mVertexSize(0)
		{
			CalculateVertexSize();
		}

		VertexBuffer::~VertexBuffer()
		{
		}

		const VertexComponent& VertexBuffer::GetVertexComponent(size_t index)
		{
			assert(index < mVertexDescriptor.components.size());
			return mVertexDescriptor.components[index];
		}

		std::size_t VertexBuffer::GetVertexComponentCount()
		{
			return mVertexDescriptor.components.size();
		}

		std::uint32_t VertexBuffer::GetVertexSize()const
		{
			return mVertexSize;
		}

		void VertexBuffer::CalculateVertexSize()
		{
			mVertexSize = 0;
			for (std::size_t i = 0;i < GetVertexComponentCount();++i)
			{
				const auto& component = GetVertexComponent(i);
				mVertexSize += GetVertexFormatSize(component.format);
			}
		}
	}
}