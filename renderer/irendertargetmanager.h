#pragma once
#include <memory>
#include <unordered_map>
#include "rendererexportdef.h"
#include "irendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderTargetManager
		{
		public:
			//create a render target
			virtual RenderTargetPtr CreateRenderTarget() = 0;
			//obtain a render target by ID
			virtual RenderTargetPtr GetRenderTarget(const RenderTargetID&) = 0;

			virtual ~IRenderTargetManager() = default;
		};
		typedef std::shared_ptr<IRenderTargetManager> SharedRenderTargetManagerPtr;

		typedef std::unordered_map<RenderTargetID, RenderTargetPtr> RenderTargetMap;
		class LIGHTNINGGE_RENDERER_API RenderTargetManager : public IRenderTargetManager
		{
		public:
			RenderTargetPtr GetRenderTarget(const RenderTargetID& targetID) override;
		protected:
			RenderTargetMap m_renderTargets;
		};
	}
}