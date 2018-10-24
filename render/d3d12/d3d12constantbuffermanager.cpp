#include <d3dx12.h>
#include "d3d12constantbuffermanager.h"
#include "renderer.h"
#include "d3d12device.h"

namespace Lightning
{
	namespace Render
	{

		D3D12ConstantBufferManager::D3D12ConstantBufferManager() 
		{

		}

		D3D12ConstantBufferManager::~D3D12ConstantBufferManager()
		{
			Clear();
		}

		//Thread unsafe
		void D3D12ConstantBufferManager::Clear()
		{
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mBufferResources[i].clear();
			}
		}

		D3D12ConstantBuffer D3D12ConstantBufferManager::AllocBuffer(std::size_t bufferSize)
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
#ifdef LIGHTNING_RENDER_MT
			auto& bufferResources = mBufferResources[resourceIndex].local();
#else
			auto& bufferResources = mBufferResources[resourceIndex];
#endif
			auto genNewBuffer = bufferResources.empty();
			if (!genNewBuffer)
			{
				auto& lastBuffer = bufferResources.back();
				if (lastBuffer.offset + bufferSize >= lastBuffer.size)
					genNewBuffer = true;
			}
			if (genNewBuffer)
			{
				BufferResource newResource;
				//D3D12 requires constant buffer having a size multiple of 256
				auto resourceSize = AlignedSize(bufferSize > MIN_BUFFER_SIZE ? bufferSize : MIN_BUFFER_SIZE, 256);
				D3D12Device* device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
				newResource.resource = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(resourceSize),
					D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
				static const CD3DX12_RANGE range(0, 0);
				//map to process virtual memory on creation to prevent mapping every time change buffer content
				newResource.resource->Map(0, &range, &newResource.mapAddress);
				newResource.offset = 0;
				newResource.size = resourceSize;
				newResource.virtualAddress = newResource.resource->GetGPUVirtualAddress();
				bufferResources.push_back(newResource);
			}
			auto realSize = AlignedSize(bufferSize, 256);
			BufferResource& resource = bufferResources.back();
			D3D12ConstantBuffer cbuffer;
			cbuffer.size = realSize;
			cbuffer.userMemory = reinterpret_cast<std::uint8_t*>(resource.mapAddress) + resource.offset;
			cbuffer.virtualAdress = resource.virtualAddress + resource.offset;
			resource.offset += realSize;

			return cbuffer;
		}

		//Thread unsafe
		void D3D12ConstantBufferManager::ResetBuffers(std::size_t frameIndex)
		{
			auto& bufferResources = mBufferResources[frameIndex];
#ifdef LIGHTNING_RENDER_MT
			for (auto it = bufferResources.begin(); it != bufferResources.end();++it)
			{
				if (it->size() > 1)
					it->resize(1);
			}
#else
			if (bufferResources.size() > 1)
			{
				bufferResources.resize(1);
			}
#endif
		}
	}
}