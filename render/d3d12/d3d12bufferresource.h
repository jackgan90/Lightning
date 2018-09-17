#pragma once
#include <d3d12.h>
#include <wrl\client.h>
#include <cstdint>
#include "rendererexportdef.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNING_RENDER_API D3D12BufferResource
		{
		public:
			D3D12BufferResource(ID3D12Device* pDevice, uint32_t bufferSize, D3D12_RESOURCE_STATES resourceState);
			ID3D12Resource* GetUploadHeap() { return mUploadHeap.Get(); }
			ID3D12Resource* GetDefaultHeap() { return mDefaultHeap.Get(); }
			D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() { return mDefaultHeap->GetGPUVirtualAddress(); }
			uint32_t GetSize()const { return mSize; }
		private:
			ComPtr<ID3D12Resource> mUploadHeap;
			ComPtr<ID3D12Resource> mDefaultHeap;
			std::uint32_t mSize;
		};
	}
}
