#include "IndexBuffer.h"

namespace Lightning
{
	namespace Render
	{
		IndexBuffer::IndexBuffer(std::uint32_t bufferSize, IndexType type)
			:mType(type), mBufferSize(bufferSize)
		{
			mIndexCount = mType == IndexType::UINT16 ? bufferSize / 2 : bufferSize / 4;
		}

		IndexType IndexBuffer::GetIndexType()const
		{
			return mType;
		}

		std::size_t IndexBuffer::GetIndexCount()const
		{
			return mIndexCount;
		}
	}
}