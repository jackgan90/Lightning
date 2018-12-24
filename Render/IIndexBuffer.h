#pragma once
#include "IGPUBuffer.h"
#include "RenderConstants.h"

namespace Lightning
{
	namespace Render
	{
		struct IIndexBuffer : IGPUBuffer
		{
			virtual IndexType INTERFACECALL GetIndexType()const = 0;
			virtual std::size_t INTERFACECALL GetIndexCount()const = 0;
		};
	}
}
