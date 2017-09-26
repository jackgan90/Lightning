#include "d3d12swapchain.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12SwapChain::D3D12SwapChain(ComPtr<IDXGISwapChain3> pSwapChain)
		{
			m_swapChain = pSwapChain;
		}

		D3D12SwapChain::~D3D12SwapChain()
		{

		}

		bool D3D12SwapChain::Present()
		{
			return true;
		}
	}
}