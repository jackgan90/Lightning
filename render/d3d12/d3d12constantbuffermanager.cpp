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
				mBufferResources[i].resource.reset();
			}
		}

		void D3D12ConstantBufferManager::Reserve(std::size_t bufferSize)
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto& bufferResource = mBufferResources[resourceIndex];
			if (bufferResource.resource && bufferResource.size >= bufferSize)
			{
				bufferResource.offset = 0;
				return;
			}
			auto resourceSize = AlignedSize(bufferSize);
			D3D12Device* device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			bufferResource.resource = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(resourceSize),
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
			static const CD3DX12_RANGE range(0, 0);
			//map to process virtual memory on creation to prevent mapping every time change buffer content
			(*bufferResource.resource)->Map(0, &range, &bufferResource.mapAddress);
			bufferResource.offset = 0;
			bufferResource.size = resourceSize;
			bufferResource.virtualAddress = (*bufferResource.resource)->GetGPUVirtualAddress();
		}

		D3D12ConstantBuffer D3D12ConstantBufferManager::AllocBuffer(std::size_t bufferSize)
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto& bufferResource = mBufferResources[resourceIndex];
			auto realSize = AlignedSize(bufferSize);
			D3D12ConstantBuffer cbuffer;
			cbuffer.size = realSize;
			auto offset = bufferResource.offset.fetch_add(realSize);
			assert(offset < bufferResource.size);
			cbuffer.userMemory = reinterpret_cast<std::uint8_t*>(bufferResource.mapAddress) + offset;
			cbuffer.virtualAdress = bufferResource.virtualAddress + offset;

			return cbuffer;
		}
	}
}