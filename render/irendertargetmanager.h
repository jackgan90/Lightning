#pragma once
#include <memory>
#include "container.h"
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

#ifdef LIGHTNING_RENDER_MT
		using RenderTargetMap = container::concurrent_unordered_map<RenderTargetID, SharedRenderTargetPtr>;
#else
		using RenderTargetMap = container::unordered_map<RenderTargetID, SharedRenderTargetPtr>;
#endif
		template<typename Derived>
		class RenderTargetManager : public IRenderTargetManager, public Foundation::Singleton<Derived>
		{
		public:
			~RenderTargetManager()override
			{
				Clear();
			}
			//Thread unsafe
			void Synchronize()
			{
#ifdef LIGHTNING_RENDER_MT
				for (auto it = mDestroyedTargetIDs.begin(); it != mDestroyedTargetIDs.end();++it)
				{
					mRenderTargets.unsafe_erase(*it);
				}
				mDestroyedTargetIDs.clear();
#endif
			}
			//Thread safe
			SharedRenderTargetPtr GetRenderTarget(const RenderTargetID targetID) override
			{
				static SharedRenderTargetPtr s_null_ptr;
#ifdef LIGHTNING_RENDER_MT
				if (mDestroyedTargetIDs.find(targetID) != mDestroyedTargetIDs.end())
					return s_null_ptr;
#endif

				auto it = mRenderTargets.find(targetID);
				if (it == mRenderTargets.end())
					return s_null_ptr;

				return it->second;
			}
			//Thread unsafe
			void Clear()
			{
				mRenderTargets.clear();
			}

			void DestroyRenderTarget(const RenderTargetID rtID)override
			{
#ifdef LIGHTNING_RENDER_MT
				mDestroyedTargetIDs.insert(rtID);
#else
				auto it = mRenderTargets.find(rtID);
				if (it != mRenderTargets.end())
					mRenderTargets.erase(it);
#endif
			}
		protected:
			//Thread safe
			void StoreRenderTarget(RenderTargetID rtID, const SharedRenderTargetPtr& ptr)
			{
				mRenderTargets[rtID] = ptr;
			}
			RenderTargetMap mRenderTargets;
#ifdef LIGHTNING_RENDER_MT
			container::concurrent_unordered_set<RenderTargetID> mDestroyedTargetIDs;
#endif
		};
	}
}
