#pragma once
#include <memory>
#include "iindexbuffer.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API IndexBuffer : public IIndexBuffer
		{
		public:
			IndexBuffer(IndexType type);
		};
	}
}
