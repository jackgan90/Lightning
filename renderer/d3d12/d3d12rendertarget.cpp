#include "d3d12rendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12RenderTarget::D3D12RenderTarget(const ComPtr<ID3D12Resource>& pRenderTarget, bool isSwapChainTarget, const RenderTargetID& rtID)
			:m_nativeRenderTarget(pRenderTarget)
			,m_isSwapChainTarget(isSwapChainTarget)
			,m_ID(rtID)
		{

		}

		D3D12RenderTarget::~D3D12RenderTarget()
		{
 
		}

		bool D3D12RenderTarget::IsSwapChainRenderTarget()const
		{
			return m_isSwapChainTarget;
		}
	}
}