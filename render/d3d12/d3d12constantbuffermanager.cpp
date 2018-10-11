#include <d3dx12.h>
#include "d3d12constantbuffermanager.h"
#include "renderer.h"
#include "d3d12device.h"

namespace Lightning
{
	namespace Render
	{

		D3D12ConstantBufferManager::D3D12ConstantBufferManager() 
			:mCurrentID(0)
		{

		}

		D3D12ConstantBufferManager::~D3D12ConstantBufferManager()
		{
			Clear();
		}

		void D3D12ConstantBufferManager::Clear()
		{
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				for (auto it = mBufferResources[i].begin(); it != mBufferResources[i].end();++it)
				{
					it->resource->Release();
				}
				mBufferResources[i].clear();
				mAllocations[i].clear();
			}
		}

		D3D12ConstantBuffer D3D12ConstantBufferManager::AllocBuffer(std::size_t bufferSize)
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto& bufferResources = mBufferResources[resourceIndex];
			if (bufferResources.empty() || bufferResources.back().offset + bufferSize >= bufferResources.back().size)
			{
				BufferResource newResource;
				auto nativeDevice = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice())->GetNative();
				//D3D12 requires constant buffer having a size multiple of 256
				auto resourceSize = AlignedSize(bufferSize > MIN_BUFFER_SIZE ? bufferSize : MIN_BUFFER_SIZE, 256);
				nativeDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(resourceSize),
					D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&newResource.resource));
				CD3DX12_RANGE range(0, 0);
				//map to process virtual memory on creation to prevent mapping everytime change buffer content
				newResource.resource->Map(0, &range, &newResource.mapAddress);
				newResource.offset = 0;
				newResource.size = resourceSize;
				newResource.virtualAddress = newResource.resource->GetGPUVirtualAddress();
				bufferResources.push_back(newResource);
			}
			auto realSize = AlignedSize(bufferSize, 256);
			BufferResource& resource = bufferResources.back();
			BufferAllocation allocation;
			allocation.offset = resource.offset;
			allocation.resource = &resource;
			allocation.size = realSize;
			mAllocations[resourceIndex][++mCurrentID] = allocation;
			resource.offset += realSize;

			D3D12ConstantBuffer cbuffer;
			cbuffer.size = realSize;
			cbuffer.userMemory = reinterpret_cast<std::uint8_t*>(resource.mapAddress) + allocation.offset;
			cbuffer.virtualAdress = resource.virtualAddress + allocation.offset;
			return cbuffer;
		}

		/*
		void D3D12ConstantBufferManager::UnlockBuffer(std::size_t bufferId, container::tuple<std::size_t, std::size_t> dirtyRange)
		{

		}*/

		void D3D12ConstantBufferManager::ResetBuffers(std::size_t frameIndex)
		{
			auto& bufferResources = mBufferResources[frameIndex];
			while (bufferResources.size() > 1)
			{
				auto& back = bufferResources.back();
				back.resource->Release();
				bufferResources.pop_back();
			}
			mAllocations[frameIndex].clear();
		}
	}
}