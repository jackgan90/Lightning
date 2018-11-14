#pragma once
#include <memory>
#include "IIndexBuffer.h"

namespace Lightning
{
	namespace Render
	{
		class IndexBuffer : public IIndexBuffer
		{
		protected:
			IndexBuffer(std::uint32_t bufferSize, IndexType type);
		};
	}
}
