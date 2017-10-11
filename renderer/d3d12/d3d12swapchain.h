#pragma once
#include <d3dx12.h>
#include <DXGI.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <unordered_map>
#include "iswapchain.h"
#include "d3d12rendertargetmanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12SwapChain : public ISwapChain
		{
		public:
			friend class D3D12Renderer;
			D3D12SwapChain(ComPtr<IDXGISwapChain3> pSwapChain, D3D12Renderer* pRenderer);
			~D3D12SwapChain()override;
			bool Present()override;
			unsigned int GetBufferCount() const override{ return m_bufferCount; }
			RenderTargetPtr GetBufferRenderTarget(unsigned int bufferIndex)override;
		private:
			void BindRenderTargets();
			unsigned int m_bufferCount;
			D3D12Renderer* m_renderer;
			ComPtr<IDXGISwapChain3> m_swapChain;
			std::unordered_map<UINT, RenderTargetID> m_renderTargets;
		};
	}
}