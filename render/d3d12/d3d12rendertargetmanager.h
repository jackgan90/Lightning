#pragma once
#include <memory>
#include <unordered_map>
#include <wrl/client.h>
#include <d3d12.h>
#include "irendertargetmanager.h"
#include "d3d12device.h"
#include "d3d12statefulresource.h"
#include <atomic>

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class D3D12SwapChain;
		class LIGHTNING_RENDER_API D3D12RenderTargetManager : public RenderTargetManager<D3D12RenderTargetManager>
		{
		public:
			D3D12RenderTargetManager();
			~D3D12RenderTargetManager()override;
			SharedRenderTargetPtr CreateRenderTarget()override;
			SharedRenderTargetPtr CreateSwapChainRenderTarget(const D3D12StatefulResourcePtr& resource, D3D12SwapChain* pSwapChain);
		private:
			std::atomic<RenderTargetID> mCurrentID;
		};
	}
}
