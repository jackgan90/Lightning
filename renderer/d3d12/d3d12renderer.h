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
			RenderContextPtr CreateRenderContext()override { return m_context; }
			void SetClearColor(const Color& color)override;
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		protected:
			void BeginRender();
			void DoRender();
			void EndRender();
		private:
			void WaitForPreviousFrame();
			RenderContextPtr m_context;
			UINT m_currentBackBufferIndex;
			Color m_clearColor;
		};
	}
}