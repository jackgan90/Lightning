#pragma once
#include "RefObject.h"
#include "VertexBuffer.h" 
#include "D3D12Device.h"
#include "D3D12BufferResource.h"

namespace Lightning
{
	namespace Render
	{
		//Thread unsafe
		class D3D12VertexBuffer : public VertexBuffer 
		{
		public:
			D3D12VertexBuffer(D3D12Device* pDevice, std::uint32_t bufferSize, const VertexDescriptor& descriptor);
			std::uint8_t* Lock(std::size_t start, std::size_t size)override { return mResource.Lock(start, size); };
			void Unlock(std::size_t start, std::size_t size)override { mResource.Unlock(start, size); };
			void Commit() override{ mResource.Commit(); };
			D3D12_VERTEX_BUFFER_VIEW GetBufferView() { return mBufferView; }
		private:
			D3D12_VERTEX_BUFFER_VIEW mBufferView;
			D3D12BufferResource mResource;
		};
	}
}
