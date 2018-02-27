#pragma once
#include <cstdint>
#include <boost/core/noncopyable.hpp>
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

		class LIGHTNINGGE_RENDER_API GPUBuffer : private boost::noncopyable
		{
		public:
			GPUBuffer();
			virtual ~GPUBuffer(){}
			//get internal data
			virtual const std::uint8_t* GetBuffer()const;
			//set internal buffer,no copy
			virtual void SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize);
			//get internal buffer size in bytes
			virtual std::uint32_t GetBufferSize()const;
			//get the buffer type
			virtual GPUBufferType GetType()const = 0;
		protected:
			std::uint8_t* m_buffer;
			std::uint32_t m_bufferSize;
		};
	}
}
