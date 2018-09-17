#include <cassert>
#include <algorithm>
#include "vertexbuffer.h"
#include "rendererhelper.h"

namespace Lightning
{
	namespace Render
	{
		VertexBuffer::VertexBuffer(uint32_t bufferSize, const VertexComponent *components, std::uint8_t componentCount):
			IVertexBuffer(bufferSize), mComponents(nullptr), mComponentCount(componentCount)
			,mVertexSize(0), mVertexCount(0)
		{
			if (mComponentCount)
			{
				mComponents = new VertexComponent[mComponentCount];
				std::memcpy(mComponents, components, mComponentCount * sizeof(VertexComponent));
			}
			CalculateVertexSize();
		}

		VertexBuffer::~VertexBuffer()
		{
			if (mComponents)
			{
				delete[] mComponents;
			}
		}

		const VertexComponent& VertexBuffer::GetComponentInfo(size_t componentIndex)
		{
			assert(componentIndex < mComponentCount);
			return mComponents[componentIndex];
		}

		std::uint8_t VertexBuffer::GetComponentCount()
		{
			return mComponentCount;
		}

		std::uint32_t VertexBuffer::GetVertexCount()const 
		{
			return mVertexCount;
		}

		std::uint32_t VertexBuffer::GetVertexSize()const
		{
			return mVertexSize;
		}


		void VertexBuffer::SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)
		{
			IVertexBuffer::SetBuffer(buffer, bufferSize);
			mVertexCount = mUsedSize / mVertexSize;
		}


		void VertexBuffer::CalculateVertexSize()
		{
			mVertexSize = 0;
			for (std::size_t i = 0;i < mComponentCount;++i)
			{
				mVertexSize += GetVertexFormatSize(mComponents[i].format);
			}
		}
	}
}