#pragma once
#include <cstdint>
#include <memory>
#include "RendererExportDef.h"
#include "RenderConstants.h"

namespace Lightning
{
	namespace Render
	{
		enum class GPUBufferType
		{
			VERTEX,
			INDEX,
		};
		

		class IGPUBuffer
		{
		public:
			virtual ~IGPUBuffer(){}
			//get internal data
			virtual std::uint8_t* Lock(std::size_t start, std::size_t size) = 0;
			//set internal buffer,no copy
			virtual void Unlock(std::size_t start, std::size_t size) = 0;
			//commit the buffer to GPU
			virtual void Commit() = 0;
			//get internal buffer size in bytes
			virtual std::uint32_t GetBufferSize()const = 0;
			//get the buffer type
			virtual GPUBufferType GetType()const = 0;
		};
		using SharedGPUBufferPtr = std::shared_ptr<IGPUBuffer>;
	}
}
