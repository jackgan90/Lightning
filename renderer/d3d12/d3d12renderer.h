#pragma once
#include "d3d12rendercontext.h"
#include "renderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API D3D12Renderer : public Renderer
		{
		public:
			D3D12Renderer(RenderContextPtr pContext);
			void ReleaseRenderResources()override;
			DevicePtr GetDevice()override;
			SwapChainPtr GetSwapChain()override;
		protected:
			void BeginRender();
			void DoRender();
			void EndRender();
		private:
			RenderContextPtr m_context;
			DevicePtr m_device;
			SwapChainPtr m_swapChain;
		};
	}
}