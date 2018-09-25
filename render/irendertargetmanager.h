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

		using RenderTargetMap = container::unordered_map<RenderTargetID, SharedRenderTargetPtr>;
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
				auto it = mRenderTargets.find(targetID);
				if (it == mRenderTargets.end())
				{
					return SharedRenderTargetPtr();
				}
				return it->second;
			}
			void Clear()
			{
				mRenderTargets.clear();
			}
			void DestroyRenderTarget(const RenderTargetID rtID)
			{
				auto it = mRenderTargets.find(rtID);
				if (it != mRenderTargets.end())
					mRenderTargets.erase(it);
			}
		protected:
			RenderTargetMap mRenderTargets;
		};
	}
}
