#include <cassert>
#include <algorithm>
#include "VertexBuffer.h"
#include "RendererHelper.h"

namespace Lightning
{
	namespace Render
	{
		VertexBuffer::VertexBuffer(uint32_t bufferSize, const VertexDescriptor& descriptor)
			: mBufferSize(bufferSize)
			, mVertexSize(0)
		{
			//Copy VertexComponent array from descriptor.
			mVertexDescriptor.components = nullptr;
			mVertexDescriptor.componentCount = descriptor.componentCount;
			if (mVertexDescriptor.componentCount > 0)
			{
				mVertexDescriptor.components = new VertexComponent[mVertexDescriptor.componentCount];
				std::memcpy(mVertexDescriptor.components, descriptor.components, sizeof(VertexComponent) * mVertexDescriptor.componentCount);
			}
			CalculateVertexSize();
		}

		VertexBuffer::~VertexBuffer()
		{
			if (mVertexDescriptor.components)
			{
				delete[] mVertexDescriptor.components;
			}
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
			for (std::size_t i = 0;i < mVertexDescriptor.componentCount;++i)
			{
				const auto& component = mVertexDescriptor.components[i];
				auto formatSize = GetVertexFormatSize(component.format);
				assert(formatSize > 0 && "VertexFormat size must be positive.");
				mVertexSize += formatSize;
			}
		}
	}
}