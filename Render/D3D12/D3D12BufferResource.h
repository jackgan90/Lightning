#pragma once
#include <wrl\client.h>
#include "D3D12Device.h"
#include "D3D12StatefulResource.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread unsafe
		class D3D12BufferResource
		{
		public:
			D3D12BufferResource(D3D12Device* pDevice, uint32_t bufferSize, GPUBufferType bufferType);
			std::uint8_t* Lock(std::size_t start, std::size_t size);
			void Unlock(std::size_t start, std::size_t size);
			void Commit();
			D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress()const { 
				return mResource->GetResource()->GetGPUVirtualAddress();
			}
		private:
			D3D12StatefulResourcePtr mIntermediateResource;
			D3D12StatefulResourcePtr mResource;
			D3D12_RANGE mDirtyRange;
			GPUBufferType mBufferType;
			void *mLockedPtr;
			std::uint32_t mSize;
		};
	}
}