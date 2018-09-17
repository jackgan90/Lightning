#include "d3d12rendertarget.h"

namespace Lightning
{
	namespace Render
	{
		D3D12RenderTarget::D3D12RenderTarget(const RenderTargetID rtID, const ComPtr<ID3D12Resource>& resource, ID3D12Device* pNativeDevice, ISwapChain* pSwapChain)
			:mResource(resource)
			,mIsSwapChainTarget(true)
			,mID(rtID)
		{
			mSampleCount = pSwapChain->GetSampleCount();
			mSampleQuality = pSwapChain->GetSampleQuality();
			mFormat = pSwapChain->GetRenderFormat();
			mHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, 1, pNativeDevice);
			pNativeDevice->CreateRenderTargetView(resource.Get(), nullptr, mHeap.cpuHandle);
		}

		D3D12RenderTarget::~D3D12RenderTarget()
		{
			mResource.Reset();
			D3D12DescriptorHeapManager::Instance()->Deallocate(mHeap.cpuHandle);
		}

		bool D3D12RenderTarget::IsSwapChainRenderTarget()const
		{
			return mIsSwapChainTarget;
		}
	}
}