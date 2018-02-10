#pragma once
#include <memory>
#include <unordered_map>
#include "rendererexportdef.h"
#include "irendertarget.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API IRenderTargetManager
		{
		public:
			//create a render target
			virtual SharedRenderTargetPtr CreateRenderTarget() = 0;
			//obtain a render target by ID
			virtual SharedRenderTargetPtr GetRenderTarget(const RenderTargetID&) = 0;

			virtual ~IRenderTargetManager() = default;
		};
		using SharedRenderTargetManagerPtr = std::shared_ptr<IRenderTargetManager>;

		using RenderTargetMap = std::unordered_map<RenderTargetID, SharedRenderTargetPtr>;
		class LIGHTNINGGE_RENDER_API RenderTargetManager : public IRenderTargetManager
		{
		public:
			SharedRenderTargetPtr GetRenderTarget(const RenderTargetID& targetID) override;
		protected:
			RenderTargetMap m_renderTargets;
		};
	}
}