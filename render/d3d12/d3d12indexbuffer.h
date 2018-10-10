#pragma once
#include "indexbuffer.h"
#include "d3d12device.h"
#include "d3d12bufferresource.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API D3D12IndexBuffer : public IndexBuffer
		{
		public:
			D3D12IndexBuffer(D3D12Device* pDevice, std::uint32_t bufferSize, IndexType type);
			std::uint8_t* Lock(std::size_t start, std::size_t size)override { return mResource.Lock(start, size);};
			void Unlock(std::size_t start, std::size_t size)override { mResource.Unlock(start, size); };
			void Commit() override{ mResource.Commit(); };
			D3D12_INDEX_BUFFER_VIEW GetBufferView() { return mBufferView; }
		private:
			D3D12_INDEX_BUFFER_VIEW mBufferView;
			D3D12BufferResource mResource;
		};
	}
}
