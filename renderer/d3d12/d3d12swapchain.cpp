#include "d3d12swapchain.h"
#include <dxgi.h>

namespace LightningGE
{
	namespace Renderer
	{
		D3D12SwapChain::D3D12SwapChain(ComPtr<IDXGISwapChain3> pSwapChain)
		{
			m_swapChain = pSwapChain;
			DXGI_SWAP_CHAIN_DESC desc;
			pSwapChain->GetDesc(&desc);
			m_bufferCount = desc.BufferCount;
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