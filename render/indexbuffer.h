#pragma once
#include <memory>
#include "iindexbuffer.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API IndexBuffer : public IIndexBuffer
		{
		protected:
			IndexBuffer(std::uint32_t bufferSize, IndexType type);
		};
	}
}
