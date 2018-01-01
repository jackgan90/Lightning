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
		class LIGHTNINGGE_RENDER_API D3D12RenderTargetManager : public RenderTargetManager
		{
		public:
			D3D12RenderTargetManager(D3D12Device* pDevice);
			~D3D12RenderTargetManager()override;
			SharedRenderTargetPtr CreateRenderTarget()override;
			SharedRenderTargetPtr CreateSwapChainRenderTarget(const ComPtr<ID3D12Resource>& swapChainRT, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
		private:
			D3D12Device* m_pDevice;
			RenderTargetID m_currentID;
		};
	}
}