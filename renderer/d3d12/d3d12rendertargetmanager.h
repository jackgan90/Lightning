#pragma once
#include "irendertargetmanager.h"
#include "idevice.h"
#include "iswapchain.h"
#include <unordered_map>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API D3D12RenderTargetManager : public IRenderTargetManager
		{
		public:
			friend class D3D12RenderContext;
			D3D12RenderTargetManager(DevicePtr pDevice, SwapChainPtr pSwapChain);
			RenderTargetPtr CreateRenderTarget()override;
			RenderTargetPtr GetRenderTarget(const RenderTargetID& targetID) override
			{
				if (m_renderTargets.find(targetID) != m_renderTargets.end())
				{
					return m_renderTargets[targetID];
				}
				return RenderTargetPtr();
			}
			void ReleaseRenderResources()override;
		private:
			DevicePtr m_pDevice;
			SwapChainPtr m_pSwapChain;
			std::unordered_map<RenderTargetID, RenderTargetPtr> m_renderTargets;
			RenderTargetID m_currentID;
		};
	}
}