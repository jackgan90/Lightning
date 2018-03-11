#include "d3d12rendertarget.h"

namespace LightningGE
{
	namespace Render
	{
		D3D12RenderTarget::D3D12RenderTarget(const RenderTargetID rtID, const ComPtr<ID3D12Resource>& resource, ISwapChain* pSwapChain)
			:m_resource(resource)
			,m_isSwapChainTarget(true)
			,m_ID(rtID)
		{
			m_sampleCount = pSwapChain->GetSampleCount();
			m_sampleQuality = pSwapChain->GetSampleQuality();
			m_format = pSwapChain->GetRenderFormat();
		}

		D3D12RenderTarget::~D3D12RenderTarget()
		{
			m_resource.Reset();
		}

		bool D3D12RenderTarget::IsSwapChainRenderTarget()const
		{
			return m_isSwapChainTarget;
		}
	}
}