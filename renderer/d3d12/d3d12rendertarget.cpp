#include "d3d12rendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12RenderTarget::D3D12RenderTarget(ComPtr<ID3D12Resource> pRenderTarget, bool isSwapChainTarget, const RenderTargetID& rtID)
			:m_nativeRenderTarget(pRenderTarget)
			,m_isSwapChainTarget(isSwapChainTarget)
			,m_ID(rtID)
		{

		}

		void D3D12RenderTarget::ReleaseRenderResources()
		{
			m_nativeRenderTarget.Reset();
		}

		bool D3D12RenderTarget::IsSwapChainRenderTarget()const
		{
			return m_isSwapChainTarget;
		}
	}
}