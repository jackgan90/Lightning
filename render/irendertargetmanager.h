#pragma once
#include <memory>
#include <unordered_map>
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "singleton.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API IRenderTargetManager
		{
		public:
			//create a render target
			virtual SharedRenderTargetPtr CreateRenderTarget() = 0;
			//obtain a render target by ID
			virtual SharedRenderTargetPtr GetRenderTarget(const RenderTargetID rtID) = 0;
			//destroy the render target identified by rtID managed by the manager
			virtual void DestroyRenderTarget(const RenderTargetID rtID) = 0;

			virtual ~IRenderTargetManager() = default;
		};
		using SharedRenderTargetManagerPtr = std::shared_ptr<IRenderTargetManager>;

		using RenderTargetMap = std::unordered_map<RenderTargetID, SharedRenderTargetPtr>;
		template<typename Derived>
		class RenderTargetManager : public IRenderTargetManager, public Foundation::Singleton<Derived>
		{
		public:
			~RenderTargetManager()override
			{
				Clear();
			}
			SharedRenderTargetPtr GetRenderTarget(const RenderTargetID targetID) override
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
			void DestroyRenderTarget(const RenderTargetID rtID)
			{
				auto it = m_renderTargets.find(rtID);
				if (it != m_renderTargets.end())
					m_renderTargets.erase(it);
			}
		protected:
			RenderTargetMap m_renderTargets;
		};
	}
}
