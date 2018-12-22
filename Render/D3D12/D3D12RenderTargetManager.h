#pragma once
#include <memory>
#include <unordered_map>
#include <wrl/client.h>
#include <d3d12.h>
#include "IRenderTargetManager.h"
#include "D3D12Device.h"
#include "D3D12StatefulResource.h"
#include <atomic>

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class D3D12SwapChain;
		class D3D12RenderTargetManager : public RenderTargetManager<D3D12RenderTargetManager>
		{
		public:
			D3D12RenderTargetManager();
			INTERFACECALL ~D3D12RenderTargetManager()override;
			IRenderTarget* INTERFACECALL CreateRenderTarget()override;
			IRenderTarget* CreateSwapChainRenderTarget(const D3D12StatefulResourcePtr& resource, D3D12SwapChain* pSwapChain);
		private:
			std::atomic<RenderTargetID> mCurrentID;
		};
	}
}
