#pragma once
#include <wrl\client.h>
#include "d3d12device.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class D3D12BufferResource
		{
		public:
			D3D12BufferResource(D3D12Device* pDevice, uint32_t bufferSize, GPUBufferType bufferType);
			std::uint8_t* Lock(std::size_t start, std::size_t size);
			void Unlock(std::size_t start, std::size_t size);
			void Commit();
			D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress()const { return mResource->GetGPUVirtualAddress(); }
		private:
			ComPtr<ID3D12Resource> mIntermediateRes;
			ComPtr<ID3D12Resource> mResource;
			D3D12_RANGE mDirtyRange;
			GPUBufferType mBufferType;
			D3D12Device* mDevice;
			void *mLockedPtr;
			std::uint32_t mSize;
		};
	}
}