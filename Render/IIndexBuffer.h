#pragma once
#include <memory>
#include "GPUBuffer.h"

namespace Lightning
{
	namespace Render
	{
		enum class IndexType
		{
			UINT16,
			UINT32,
		};

		class LIGHTNING_RENDER_API IIndexBuffer : public GPUBuffer
		{
		public:
			IIndexBuffer(std::uint32_t bufferSize, IndexType type) : GPUBuffer(bufferSize), mType(type){}
			IndexType GetIndexType()const { return mType; }
			GPUBufferType GetType()const override { return GPUBufferType::INDEX; }
			std::size_t GetIndexCount()const { return mType == IndexType::UINT16 ? mBufferSize / 2 : mBufferSize / 4; }
		protected:
			IndexType mType;
		};
		using SharedIndexBufferPtr = std::shared_ptr<IIndexBuffer>;
	}
}
