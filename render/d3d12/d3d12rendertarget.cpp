#include "d3d12rendertarget.h"
#include "renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12RenderTarget::D3D12RenderTarget(const RenderTargetID rtID, const D3D12StatefulResourcePtr& resource, ISwapChain* pSwapChain)
			:mResource(resource)
			,mIsSwapChainTarget(true)
			,mID(rtID)
		{
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			mSampleCount = pSwapChain->GetSampleCount();
			mSampleQuality = pSwapChain->GetSampleQuality();
			mFormat = pSwapChain->GetRenderFormat();
			mHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, 1, false);
			device->CreateRenderTargetView(*resource, nullptr, mHeap->cpuHandle);
		}

		D3D12RenderTarget::~D3D12RenderTarget()
		{
			D3D12DescriptorHeapManager::Instance()->Deallocate(mHeap);
			mHeap = nullptr;
		}

		bool D3D12RenderTarget::IsSwapChainRenderTarget()const
		{
			return mIsSwapChainTarget;
		}
	}
}