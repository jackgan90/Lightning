#include <d3dx12.h>
#include "D3D12DepthStencilBuffer.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12TypeMapper.h"
#include "Renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12DepthStencilBuffer::D3D12DepthStencilBuffer(D3D12Texture* texture, 
			float depthClearValue, std::uint8_t stencilClearValue)
			: mDepthClearValue(depthClearValue)
			, mStencilClearValue(stencilClearValue)
			, mHeap(nullptr)
			, mTexture(texture)
		{
			assert(mTexture != nullptr && "mTexture cannot be nullptr!");
			mTexture->AddRef();
			CreateResource();
		}

		void D3D12DepthStencilBuffer::CreateResource()
		{
			//create heap
			mHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, 1, false);


			auto device = mTexture->GetDevice();

			auto resource = mTexture->GetResource();
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = D3D12TypeMapper::MapRenderFormat(mTexture->GetRenderFormat());
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			device->CreateDepthStencilView((*resource), &dsvDesc, mHeap->cpuHandle);
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

		float D3D12DepthStencilBuffer::GetDepthClearValue()const
		{
			return mDepthClearValue;
		}

		std::uint8_t D3D12DepthStencilBuffer::GetStencilClearValue()const
		{
			return mStencilClearValue;
		}

		void D3D12DepthStencilBuffer::TransitToState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state)
		{
			mTexture->GetResource()->TransitTo(commandList, state);
		}
	}
}
