#include <d3dx12.h>
#include "D3D12ConstantBufferManager.h"
#include "Renderer.h"
#include "D3D12Device.h"

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
				mBufferResources[i].for_each([](std::vector<BufferResource>& bufferResources) {
					bufferResources.clear();
				});
			}
		}

		D3D12ConstantBufferManager::BufferResource D3D12ConstantBufferManager::Reserve(std::size_t bufferSize)
		{
			assert(bufferSize > 0 && "bufferSize must be a positive value!");
			BufferResource bufferResource;
			auto resourceSize = std::max(AlignedSize(bufferSize), MIN_BUFFER_SIZE);
			D3D12Device* device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			bufferResource.resource = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(resourceSize),
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
			static const CD3DX12_RANGE range(0, 0);
			//map to process virtual memory on creation to prevent mapping every time change buffer content
			bufferResource.resource->GetResource()->Map(0, &range, &bufferResource.mapAddress);
			bufferResource.offset = 0;
			bufferResource.size = resourceSize;
			bufferResource.virtualAddress = bufferResource.resource->GetResource()->GetGPUVirtualAddress();
			bufferResource.frameCount = Renderer::Instance()->GetCurrentFrameCount();
			return bufferResource;
		}

		D3D12ConstantBuffer D3D12ConstantBufferManager::AllocBuffer(std::size_t bufferSize)
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto frameCount = Renderer::Instance()->GetCurrentFrameCount();
			auto& bufferResources = mBufferResources[resourceIndex];
			auto& threadBufferResources = *bufferResources;

			auto realSize = AlignedSize(bufferSize);
			D3D12ConstantBuffer cbuffer;
			cbuffer.size = realSize;
			
			if (threadBufferResources.empty() || threadBufferResources.back().frameCount != frameCount)
			{
				//It's another new frame,we need to merge buffers of previous frame
				if (threadBufferResources.size() == 1 && threadBufferResources.back().size >= realSize)//Try to reuse buffers allocated on previous frame,if its size is big enough
				{
					auto& bufferResource = threadBufferResources.back();
					bufferResource.frameCount = frameCount;
					bufferResource.offset = 0;
				}
				else			//need to merge all buffers into one bigger buffer and make sure the merged buffer is big enough
				{
					std::size_t totalBufferSize{ realSize };
					std::for_each(threadBufferResources.cbegin(), threadBufferResources.cend(), 
						[&totalBufferSize](const BufferResource& bufferResource) {
						totalBufferSize += bufferResource.size;
					});
					threadBufferResources.clear();
					threadBufferResources.emplace_back(Reserve(totalBufferSize));
				}
			}
			auto offset = threadBufferResources.back().offset;
			if (offset + realSize >= threadBufferResources.back().size)
			{
				threadBufferResources.emplace_back(Reserve(realSize));
				offset = 0;
			}
			assert(offset + realSize < threadBufferResources.back().size);
			cbuffer.userMemory = reinterpret_cast<std::uint8_t*>(threadBufferResources.back().mapAddress) + offset;
			cbuffer.virtualAdress = threadBufferResources.back().virtualAddress + offset;
			threadBufferResources.back().offset += realSize;

			return cbuffer;
		}
	}
}