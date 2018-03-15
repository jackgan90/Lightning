#pragma once
#include <boost/core/noncopyable.hpp>
#include "igpubuffer.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API GPUBuffer : public IGPUBuffer, private boost::noncopyable
		{
		public:
			GPUBuffer();
			//get internal data
			const std::uint8_t* GetBuffer()const override;
			//set internal buffer,no copy
			void SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)override;
			//get internal buffer size in bytes
			std::uint32_t GetBufferSize()const override;
		protected:
			std::uint8_t* m_buffer;
			std::uint32_t m_bufferSize;
		};
	}
}
