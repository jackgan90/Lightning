#pragma once
#include "irendercontext.h"
#include "irendertargetmanager.h"
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl\client.h>


namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12RenderContext : public IRenderContext
		{
		public:
			~D3D12RenderContext()override;
			bool Init(WindowPtr pWindow)override;
			//entry method of render system
			void Render()override;
			DevicePtr GetDevice()override;
			SwapChainPtr GetSwapChain()override;
		private:
			bool InitDevice(ComPtr<IDXGIFactory4> dxgiFactory);
			bool InitSwapChain(ComPtr<IDXGIFactory4> dxgiFactory, WindowPtr pWindow);
			bool BindSwapChainRenderTargets();
			DevicePtr m_device;
			SwapChainPtr m_swapChain;
			RenderTargetManagerPtr m_renderTargetManager;
			ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		};
	}
}