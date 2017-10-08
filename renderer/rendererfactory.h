#pragma once
#include "rendererexportdef.h"
#include "idevice.h"
#include "iswapchain.h"
#include "irendercontext.h"
#include "irendertargetmanager.h"
#include "ishadermanager.h"
#include "irenderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API RendererFactory
		{
		public:
			//create a singleton render target manager of the implementation
			static RenderTargetManagerPtr CreateRenderTargetManager(DevicePtr pDevice, SwapChainPtr pSwapChain);
			//TODO some graphics API may coexist more than one context in the application process,try to resovle the condition
			//create a render context which can be used to initialize rendering pipeline
			static RenderContextPtr CreateRenderContext();
			//create a singleton renderer to control the rendering workflow
			static RendererPtr CreateRenderer(RenderContextPtr pRenderContext);
			//create a singleton shader manager
			static ShaderManagerPtr CreateShaderManager();
			//return an already created render target manager instance
			static RenderTargetManagerPtr GetRenderTargetManager() { return s_renderTargetMgr; }
			//return an already created render context
			static RenderContextPtr GetRenderContext() { return s_renderContext; }
			//return an already created renderer
			static RendererPtr GetRenderer() { return s_renderer; }
			//return a shader manager
			static ShaderManagerPtr GetShaderManager() { return s_shaderMgr; }
			//finalize renderer factory, should be called only at the end of the app
			static void Finalize();
		private:
			static RenderTargetManagerPtr s_renderTargetMgr;
			static RenderContextPtr s_renderContext;
			static RendererPtr s_renderer;
			static ShaderManagerPtr s_shaderMgr;
		};
	}
}