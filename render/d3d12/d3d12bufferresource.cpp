#include <d3dx12.h>
#include "d3d12bufferresource.h"

namespace LightningGE
{
	namespace Render
	{
		D3D12BufferResource::D3D12BufferResource(ID3D12Device* pDevice, uint32_t bufferSize):m_size(bufferSize)
		{
			pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
						D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
						D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_defaultHeap));
			pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
						D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
						D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_uploadHeap));
			m_accessState = D3D12_RESOURCE_STATE_COPY_DEST;
		}
	}

}