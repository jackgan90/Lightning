#pragma once
#include <memory>
#include <unordered_map>
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "singleton.h"

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
		template<typename Derived>
		class RenderTargetManager : public IRenderTargetManager, public Foundation::Singleton<Derived>
		{
		public:
			SharedRenderTargetPtr GetRenderTarget(const RenderTargetID& targetID) override
			{
				auto it = m_renderTargets.find(targetID);
				if (it == m_renderTargets.end())
				{
					return SharedRenderTargetPtr();
				}
				return it->second;
			}
			void Clear()
			{
				m_renderTargets.clear();
			}
		protected:
			RenderTargetMap m_renderTargets;
		};
	}
}