#include <d3dx12.h>
#include "d3d12bufferresource.h"

namespace Lightning
{
	namespace Render
	{
		D3D12BufferResource::D3D12BufferResource(ID3D12Device* pDevice, uint32_t bufferSize, D3D12_RESOURCE_STATES resourceState)
			:mSize(bufferSize)
		{
			pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
						D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
						resourceState, nullptr, IID_PPV_ARGS(&mDefaultHeap));
			pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
						D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
						D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mUploadHeap));
		}
	}

}