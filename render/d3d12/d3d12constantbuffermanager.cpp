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
				mBufferResources[i].clear();
				mAllocations[i].clear();
			}
		}

		std::size_t D3D12ConstantBufferManager::AllocBuffer(std::size_t bufferSize)
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto& bufferResources = mBufferResources[resourceIndex];
			if (bufferResources.empty() || bufferResources.back().offset + bufferSize >= bufferResources.back().size)
			{
				BufferResource newResource;
				auto nativeDevice = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice())->GetNative();
				//D3D12 requires constant buffer having a size multiple of 256
				auto resourceSize = AlignedSize(bufferSize, 256);
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
			BufferResource& resource = bufferResources.back();
			BufferAllocation allocation;
			allocation.offset = resource.offset;
			allocation.resource = &resource;
			mAllocations[resourceIndex][++mCurrentID] = allocation;
			resource.offset += AlignedSize(bufferSize, 256);

			return mCurrentID;
		}

		std::uint8_t* D3D12ConstantBufferManager::LockBuffer(std::size_t bufferId)
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto it = mAllocations[resourceIndex].find(bufferId);
			if (it == mAllocations[resourceIndex].end())
			{
				return nullptr;
			}
			auto pBufferResource = it->second.resource;
			return reinterpret_cast<std::uint8_t*>(pBufferResource->mapAddress) + it->second.offset;
		}

		/*
		void D3D12ConstantBufferManager::UnlockBuffer(std::size_t bufferId, container::tuple<std::size_t, std::size_t> dirtyRange)
		{

		}*/

		D3D12_GPU_VIRTUAL_ADDRESS D3D12ConstantBufferManager::GetVirtualAddress(std::size_t bufferId)
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto it = mAllocations[resourceIndex].find(bufferId);
			if (it == mAllocations[resourceIndex].end())
			{
				return D3D12_GPU_VIRTUAL_ADDRESS(0);
			}
			auto pBufferResource = it->second.resource;
			return pBufferResource->virtualAddress + it->second.offset;
		}

		std::size_t D3D12ConstantBufferManager::GetBufferSize(std::size_t bufferId)
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto it = mAllocations[resourceIndex].find(bufferId);
			if (it == mAllocations[resourceIndex].end())
			{
				return 0;
			}
			auto pBufferResource = it->second;
			return pBufferResource.resource->size;
		}

		void D3D12ConstantBufferManager::ResetBuffers(std::size_t frameIndex)
		{
			auto& bufferResources = mBufferResources[frameIndex];
			while (bufferResources.size() > 1)
			{
				bufferResources.erase(bufferResources.begin() + bufferResources.size() - 1);
			}
			if (!bufferResources.empty())
			{
				bufferResources[0].offset = 0;
			}
			mAllocations[frameIndex].clear();
		}
	}
}