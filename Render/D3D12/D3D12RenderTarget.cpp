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
			device->CreateRenderTargetView(*resource, nullptr, mHeap->cpuHandle);
		}

		D3D12RenderTarget::~D3D12RenderTarget()
		{
			D3D12DescriptorHeapManager::Instance()->Deallocate(mHeap);
			mTexture->Release();
			mHeap = nullptr;
		}

		void D3D12RenderTarget::TransitToRTState(ID3D12GraphicsCommandList* commandList)
		{
			mTexture->GetResource()->TransitTo(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		void D3D12RenderTarget::TransitToPresentState(ID3D12GraphicsCommandList* commandList)
		{
			mTexture->GetResource()->TransitTo(commandList, D3D12_RESOURCE_STATE_PRESENT);
		}
	}
}