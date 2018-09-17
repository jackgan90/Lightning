#pragma once
#include "vertexbuffer.h"
#include "d3d12bufferresource.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API D3D12VertexBuffer : public VertexBuffer
		{
		public:
			D3D12VertexBuffer(ID3D12Device* pDevice, std::uint32_t bufferSize, const VertexComponent *components, std::uint8_t componentCount);
			D3D12_VERTEX_BUFFER_VIEW GetBufferView() { return mBufferView; }
			ComPtr<ID3D12Resource> GetResource() { return mResource.GetDefaultHeap(); }
			ComPtr<ID3D12Resource> GetIntermediateResource() { return mResource.GetUploadHeap(); }
		private:
			D3D12BufferResource mResource;
			D3D12_VERTEX_BUFFER_VIEW mBufferView;
		};
	}
}
