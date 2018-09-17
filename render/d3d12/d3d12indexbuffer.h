#pragma once
#include "d3d12bufferresource.h"
#include "indexbuffer.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API D3D12IndexBuffer : public IndexBuffer
		{
		public:
			D3D12IndexBuffer(ID3D12Device* pDevice, std::uint32_t bufferSize, IndexType type);
			D3D12_INDEX_BUFFER_VIEW GetBufferView() { return mBufferView; }
			ComPtr<ID3D12Resource> GetResource() { return mResource.GetDefaultHeap(); }
			ComPtr<ID3D12Resource> GetIntermediateResource() { return mResource.GetUploadHeap(); }
		private:
			D3D12BufferResource mResource;
			D3D12_INDEX_BUFFER_VIEW mBufferView;
		};
	}
}
