#pragma once
#include <boost/core/noncopyable.hpp>
#include "igpubuffer.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API GPUBuffer : public IGPUBuffer, private boost::noncopyable
		{
		public:
			GPUBuffer(std::uint32_t bufferSize);
			~GPUBuffer()override;
			//get internal data
			std::uint8_t* Lock(std::size_t start, std::size_t size)override;
			//set internal buffer,no copy
			void Unlock(std::size_t start, std::size_t size)override;
			//get internal buffer size in bytes
			std::uint32_t GetBufferSize()const override;
			bool IsDirty()override { return mDirty; }
			void ResetDirty()override { mDirty = false; }
			const std::uint8_t* GetBuffer()const override { return mBuffer; }
		protected:
			std::uint8_t* mBuffer;
			std::uint32_t mBufferSize;
			bool mDirty;
		};
	}
}
