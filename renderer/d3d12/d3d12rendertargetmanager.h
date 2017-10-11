#pragma once
#include <unordered_map>
#include <wrl/client.h>
#include <d3d12.h>
#include "irendertargetmanager.h"
#include "d3d12device.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12RenderTargetManager : public RenderTargetManager
		{
		public:
			D3D12RenderTargetManager(D3D12Device* pDevice);
			~D3D12RenderTargetManager()override;
			RenderTargetPtr CreateRenderTarget()override;
			RenderTargetPtr CreateSwapChainRenderTarget(ComPtr<ID3D12Resource> swapChainRT, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
		private:
			D3D12Device* m_pDevice;
			RenderTargetID m_currentID;
		};
	}
}