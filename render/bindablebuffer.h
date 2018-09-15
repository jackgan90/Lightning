#pragma once
#include <cstdint>
#include "gpubuffer.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API BindableBuffer : public GPUBuffer
		{
		public:
			BindableBuffer();
			//set binding location for this vertex buffer.The buffer will be bound to loc-th slot when issue draw call
			virtual void SetGPUBindSlot(std::uint16_t loc);
			//get binding location.Typically invoked by device to bind the buffer
			virtual std::uint16_t GetGPUBindSlot()const;
		protected:
			std::uint16_t m_GPUBindSlot;
		};
	}
}
