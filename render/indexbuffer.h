#pragma once
#include <memory>
#include "bindablebuffer.h"

namespace LightningGE
{
	namespace Render
	{
		enum class IndexType
		{
			UINT16,
			UINT32,
		};

		class LIGHTNINGGE_RENDER_API IndexBuffer : public BindableBuffer
		{
		public:
			IndexBuffer(IndexType type);
			IndexType GetIndexType()const { return m_type; }
			GPUBufferType GetType()const override { return GPUBufferType::INDEX; }
		private:
			IndexType m_type;
		};
		using SharedIndexBufferPtr = std::shared_ptr<IndexBuffer>;
	}
}
