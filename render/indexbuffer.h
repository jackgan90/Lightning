#pragma once
#include "bindablebuffer.h"

namespace LightningGE
{
	namespace Render
	{
		enum IndexType
		{
			INDEX_TYPE_UINT16,
			INDEX_TYPE_UINT32,
		};

		class LIGHTNINGGE_RENDER_API IndexBuffer : public BindableBuffer
		{
		public:
			IndexBuffer(IndexType type);
			IndexType GetIndexType()const { return m_type; }
		private:
			IndexType m_type;
		};
	}
}
