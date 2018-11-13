#include "bindablebuffer.h"

namespace Lightning
{
	namespace Render
	{
		BindableBuffer::BindableBuffer() :m_GPUBindSlot(0xffff)
		{
			
		}

		void BindableBuffer::SetGPUBindSlot(std::uint16_t loc)
		{
			m_GPUBindSlot = loc;
		}

		std::uint16_t BindableBuffer::GetGPUBindSlot()const
		{
			return m_GPUBindSlot;
		}
	}
}
