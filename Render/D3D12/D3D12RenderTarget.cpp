#include "D3D12RenderTarget.h"
#include "Renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12RenderTarget::D3D12RenderTarget(const RenderTargetID rtID, D3D12Texture* texture)
			:mTexture(texture)
			,mID(rtID)
		{
			assert(texture != nullptr && "The texture used to initialize render target cannot be nullptr!");
			mTexture->AddRef();
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			mHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, 1, false);
			auto resource = mTexture->GetResource();
			device->CreateRenderTargetView(resource->GetResource(), nullptr, mHeap->CPUHandle);
		}

		D3D12RenderTarget::~D3D12RenderTarget()
		{
			if (mHeap)
			{
				D3D12DescriptorHeapManager::Instance()->Deallocate(mHeap);
				mHeap = nullptr;
			}
			mTexture->Release();
		}

		void D3D12RenderTarget::TransitToRTState(ID3D12GraphicsCommandList* commandList)
		{
			mTexture->GetResource()->TransitTo(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		void D3D12RenderTarget::TransitToPresentState(ID3D12GraphicsCommandList* commandList)
		{
			mTexture->GetResource()->TransitTo(commandList, D3D12_RESOURCE_STATE_PRESENT);
		}

		void D3D12RenderTarget::Reset()
		{
			mTexture->GetResource()->Reset();
			if (mHeap)
			{
				D3D12DescriptorHeapManager::Instance()->Deallocate(mHeap);
				mHeap = nullptr;
			}
		}

		void D3D12RenderTarget::Reset(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES state)
		{
			mTexture->GetResource()->Reset(resource, state);
			if (mHeap)
			{
				D3D12DescriptorHeapManager::Instance()->Deallocate(mHeap);
				mHeap = nullptr;
			}
			mHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, 1, false);
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			device->CreateRenderTargetView(resource.Get(), nullptr, mHeap->CPUHandle);
		}
	}
}