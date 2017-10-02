#include "d3d12rendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12RenderTarget::D3D12RenderTarget(ComPtr<ID3D12Resource> pRenderTarget) :m_nativeRenderTarget(pRenderTarget)
		{

		}

		void D3D12RenderTarget::ReleaseRenderResources()
		{
			m_nativeRenderTarget.Reset();
		}
	}
}