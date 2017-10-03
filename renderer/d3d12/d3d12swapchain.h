#pragma once
#include <d3dx12.h>
#include <DXGI.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <unordered_map>
#include "iswapchain.h"
#include "d3d12device.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12SwapChain : public ISwapChain
		{
		public:
			friend class D3D12RenderContext;
			friend class D3D12Renderer;
			D3D12SwapChain(ComPtr<IDXGISwapChain3> pSwapChain, D3D12Device* pDevice);
			~D3D12SwapChain()override;
			bool Present()override;
			void ReleaseRenderResources()override;
			unsigned int GetBufferCount() const override{ return m_bufferCount; }
			bool BindSwapChainRenderTargets();
		private:
			unsigned int m_bufferCount;
			ComPtr<IDXGISwapChain3> m_swapChain;
			D3D12Device* m_device;
		};
	}
}