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
			virtual SharedRenderTargetPtr CreateRenderTarget() = 0;
			//obtain a render target by ID
			virtual SharedRenderTargetPtr GetRenderTarget(const RenderTargetID&) = 0;

			virtual ~IRenderTargetManager() = default;
		};
		typedef std::shared_ptr<IRenderTargetManager> SharedRenderTargetManagerPtr;

		typedef std::unordered_map<RenderTargetID, SharedRenderTargetPtr> RenderTargetMap;
		class LIGHTNINGGE_RENDERER_API RenderTargetManager : public IRenderTargetManager
		{
		public:
			SharedRenderTargetPtr GetRenderTarget(const RenderTargetID& targetID) override;
		protected:
			RenderTargetMap m_renderTargets;
		};
	}
}