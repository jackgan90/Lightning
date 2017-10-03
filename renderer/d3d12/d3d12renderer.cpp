#include "d3d12renderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12Renderer::D3D12Renderer(RenderContextPtr pContext) :m_context(pContext)
		{

		}

		void D3D12Renderer::ReleaseRenderResources()
		{
			m_context->ReleaseRenderResources();
		}

		void D3D12Renderer::BeginRender()
		{

		}

		void D3D12Renderer::DoRender()
		{

		}

		void D3D12Renderer::EndRender()
		{

		}

		DevicePtr D3D12Renderer::GetDevice()
		{
			return m_device;
		}

		SwapChainPtr D3D12Renderer::GetSwapChain()
		{
			return m_swapChain;
		}

	}
}