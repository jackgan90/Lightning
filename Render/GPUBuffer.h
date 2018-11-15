#pragma once
#include <boost/core/noncopyable.hpp>
#include "IGPUBuffer.h"

namespace Lightning
{
	namespace Render
	{
		class GPUBuffer : public IGPUBuffer, private boost::noncopyable
		{
		public:
			GPUBuffer(std::uint32_t bufferSize);
			//get internal buffer size in bytes
			std::uint32_t GetBufferSize()const override;
		protected:
			std::uint32_t mBufferSize;
		};
	}
}