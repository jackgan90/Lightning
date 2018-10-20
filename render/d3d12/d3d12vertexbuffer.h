#pragma once
#include "vertexbuffer.h" 
#include "d3d12device.h"
#include "d3d12bufferresource.h"

namespace Lightning
{
	namespace Render
	{
		//Thread unsafe
		class LIGHTNING_RENDER_API D3D12VertexBuffer : public VertexBuffer 
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
