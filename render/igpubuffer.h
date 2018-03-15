#pragma once
#include <cstdint>
#include "rendererexportdef.h"
#include "renderconstants.h"

namespace LightningGE
{
	namespace Render
	{
		enum class GPUBufferType
		{
			VERTEX,
			INDEX,
		};
		

		class LIGHTNINGGE_RENDER_API IGPUBuffer
		{
		public:
			virtual ~IGPUBuffer(){}
			//get internal data
			virtual const std::uint8_t* GetBuffer()const = 0;
			//set internal buffer,no copy
			virtual void SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize) = 0;
			//get internal buffer size in bytes
			virtual std::uint32_t GetBufferSize()const = 0;
			//get the buffer type
			virtual GPUBufferType GetType()const = 0;
		};
	}
}
