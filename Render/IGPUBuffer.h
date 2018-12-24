#pragma once
#include <cstdint>
#include "RenderConstants.h"
#include "IRefObject.h"
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		struct IGPUBuffer : Plugins::IRefObject
		{
			//get internal data
			virtual std::uint8_t* INTERFACECALL Lock(std::size_t start, std::size_t size) = 0;
			//set internal buffer,no copy
			virtual void INTERFACECALL Unlock(std::size_t start, std::size_t size) = 0;
			//commit the buffer to GPU
			virtual void INTERFACECALL Commit() = 0;
			//get internal buffer size in bytes
			virtual std::uint32_t INTERFACECALL GetBufferSize()const = 0;
			//get the buffer type
			virtual GPUBufferType INTERFACECALL GetType()const = 0;
		};
	}
}
