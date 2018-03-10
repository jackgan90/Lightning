#pragma once
#include <memory>
#include "gpubuffer.h"

namespace LightningGE
{
	namespace Render
	{
		enum class IndexType
		{
			UINT16,
			UINT32,
		};

		class LIGHTNINGGE_RENDER_API IndexBuffer : public GPUBuffer
		{
		public:
			IndexBuffer(IndexType type);
			IndexType GetIndexType()const { return m_type; }
			GPUBufferType GetType()const override { return GPUBufferType::INDEX; }
			std::size_t GetIndexCount()const { return m_type == IndexType::UINT16 ? m_bufferSize / 2 : m_bufferSize / 4; }
		private:
			IndexType m_type;
		};
		using SharedIndexBufferPtr = std::shared_ptr<IndexBuffer>;
	}
}
