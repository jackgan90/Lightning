#pragma once
#include <memory>
#include "iindexbuffer.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API IndexBuffer : public IIndexBuffer
		{
		protected:
			IndexBuffer(std::uint32_t bufferSize, IndexType type);
		};
	}
}
