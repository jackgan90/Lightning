#pragma once
#include <memory>
#include "IIndexBuffer.h"

namespace Lightning
{
	namespace Render
	{
		class IndexBuffer : public IIndexBuffer
		{
		public:
			IndexType GetIndexType()const override;
			std::size_t GetIndexCount()const override;
			GPUBufferType GetType()const override { return GPUBufferType::INDEX; }
			std::uint32_t GetBufferSize()const override { return mBufferSize; }
		protected:
			IndexBuffer(std::uint32_t bufferSize, IndexType type);
			IndexType mType;
			std::size_t mIndexCount;
			std::uint32_t mBufferSize;
		};
	}
}
