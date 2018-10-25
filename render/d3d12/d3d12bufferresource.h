#pragma once
#include <wrl\client.h>
#include "d3d12device.h"
#include "d3d12statefulresource.h"

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
				return (*mResource)->GetGPUVirtualAddress();
			}
		private:
			D3D12StatefulResourcePtr mIntermediateRes;
			D3D12StatefulResourcePtr mResource;
			D3D12_RANGE mDirtyRange;
			GPUBufferType mBufferType;
			D3D12Device* mDevice;
			void *mLockedPtr;
			std::uint32_t mSize;
		};
	}
}