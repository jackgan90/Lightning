#include <cassert>
#include <algorithm>
#include "VertexBuffer.h"
#include "RendererHelper.h"

namespace Lightning
{
	namespace Render
	{
		VertexBuffer::VertexBuffer(uint32_t bufferSize, const VertexDescriptor& descriptor)
			: mVertexDescriptor(descriptor)
			, mBufferSize(bufferSize)
			, mVertexSize(0)
		{
			CalculateVertexSize();
		}

		VertexComponent VertexBuffer::GetVertexComponent(size_t index)const
		{
			assert(index < mVertexDescriptor.componentCount);
			return mVertexDescriptor.components[index];
		}

		std::size_t VertexBuffer::GetVertexComponentCount()const
		{
			return mVertexDescriptor.componentCount;
		}

		std::uint32_t VertexBuffer::GetVertexSize()const
		{
			return mVertexSize;
		}

		std::uint32_t VertexBuffer::GetBufferSize()const
		{
			return mBufferSize;
		}

		void VertexBuffer::CalculateVertexSize()
		{
			mVertexSize = 0;
			for (std::size_t i = 0;i < GetVertexComponentCount();++i)
			{
				const auto component = GetVertexComponent(i);
				auto formatSize = GetVertexFormatSize(component.format);
				assert(formatSize > 0 && "VertexFormat size must be positive.");
				mVertexSize += formatSize;
			}
		}
	}
}