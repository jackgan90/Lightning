#pragma once
#include <cstdint>
#include <memory>
#include "rendererexportdef.h"
#include "renderconstants.h"

namespace Lightning
{
	namespace Render
	{
		enum class GPUBufferType
		{
			VERTEX,
			INDEX,
		};
		

		class LIGHTNING_RENDER_API IGPUBuffer
		{
		public:
			virtual ~IGPUBuffer(){}
			virtual const std::uint8_t* GetBuffer()const = 0;
			//get internal data
			virtual std::uint8_t* Lock(std::size_t start, std::size_t size) = 0;
			//set internal buffer,no copy
			virtual void Unlock(std::size_t start, std::size_t size) = 0;
			//return whether the underlying buffer data has changed
			virtual bool IsDirty() = 0;
			//Reset dirty flag
			virtual void ResetDirty() = 0;
			//get internal buffer size in bytes
			virtual std::uint32_t GetBufferSize()const = 0;
			//get the buffer type
			virtual GPUBufferType GetType()const = 0;
		};
		using SharedGPUBufferPtr = std::shared_ptr<IGPUBuffer>;
	}
}
