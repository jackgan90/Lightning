#include <d3dx12.h>
#include "D3D12DepthStencilBuffer.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12TypeMapper.h"
#include "Renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12DepthStencilBuffer::D3D12DepthStencilBuffer(D3D12Texture* texture)
			: mTexture(texture)
			, mHeap(nullptr)
		{
			assert(mTexture != nullptr && "mTexture cannot be nullptr!");
			mTexture->AddRef();
			CreateDepthStencilView();
		}

		void D3D12DepthStencilBuffer::CreateDepthStencilView()
		{
			//create heap
			mHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, 1, false);


			auto device = mTexture->GetDevice();

			auto resource = mTexture->GetResource();
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = D3D12TypeMapper::MapRenderFormat(mTexture->GetRenderFormat());
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			device->CreateDepthStencilView(resource->GetResource(), &dsvDesc, mHeap->cpuHandle);
		}

		D3D12DepthStencilBuffer::~D3D12DepthStencilBuffer()
		{
			mTexture->Release();
			D3D12DescriptorHeapManager::Instance()->Deallocate(mHeap);
			mHeap = nullptr;
		}

		void D3D12DepthStencilBuffer::SetClearValue(float depthValue, std::uint32_t stencilValue)
		{

		}

		void D3D12DepthStencilBuffer::Resize(std::size_t width, std::size_t height)
		{
			mTexture->Resize(width, height);
			D3D12DescriptorHeapManager::Instance()->Deallocate(mHeap);
			CreateDepthStencilView();
		}

		void D3D12DepthStencilBuffer::TransitToState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state)
		{
			mTexture->GetResource()->TransitTo(commandList, state);
		}
	}
}
