#include "d3d12rendertarget.h"

namespace Lightning
{
	namespace Render
	{
		D3D12RenderTarget::D3D12RenderTarget(const RenderTargetID rtID, const ComPtr<ID3D12Resource>& resource, ID3D12Device* pNativeDevice, ISwapChain* pSwapChain)
			:m_resource(resource)
			,m_isSwapChainTarget(true)
			,m_ID(rtID)
		{
			m_sampleCount = pSwapChain->GetSampleCount();
			m_sampleQuality = pSwapChain->GetSampleQuality();
			m_format = pSwapChain->GetRenderFormat();
			m_heap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, 1, pNativeDevice);
			pNativeDevice->CreateRenderTargetView(resource.Get(), nullptr, m_heap.cpuHandle);
		}

		D3D12RenderTarget::~D3D12RenderTarget()
		{
			m_resource.Reset();
			D3D12DescriptorHeapManager::Instance()->Deallocate(m_heap.cpuHandle);
		}

		bool D3D12RenderTarget::IsSwapChainRenderTarget()const
		{
			return m_isSwapChainTarget;
		}
	}
}