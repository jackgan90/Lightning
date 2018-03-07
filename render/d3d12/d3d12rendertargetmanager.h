#pragma once
#include <memory>
#include <unordered_map>
#include <wrl/client.h>
#include <d3d12.h>
#include "irendertargetmanager.h"
#include "d3d12device.h"

namespace LightningGE
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class D3D12SwapChain;
		class LIGHTNINGGE_RENDER_API D3D12RenderTargetManager : public RenderTargetManager<D3D12RenderTargetManager>
		{
		public:
			D3D12RenderTargetManager();
			~D3D12RenderTargetManager()override;
			SharedRenderTargetPtr CreateRenderTarget()override;
			SharedRenderTargetPtr CreateSwapChainRenderTarget(const ComPtr<ID3D12Resource>& swapChainRT, 
				const D3D12_CPU_DESCRIPTOR_HANDLE& handle, const D3D12SwapChain* pSwapChain);
		private:
			RenderTargetID m_currentID;
		};
	}
}