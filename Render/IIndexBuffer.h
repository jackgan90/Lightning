#pragma once
#include "IGPUBuffer.h"
#include "RenderConstants.h"

namespace Lightning
{
	namespace Render
	{
		struct IIndexBuffer : IGPUBuffer
		{
			virtual IndexType GetIndexType()const = 0;
			virtual std::size_t GetIndexCount()const = 0;
		};
	}
}
