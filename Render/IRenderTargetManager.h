#pragma once
#include <memory>
#include "Container.h"
#include "IRenderTarget.h"
#include "Singleton.h"

namespace Lightning
{
	namespace Render
	{
		class IRenderTargetManager
		{
		public:
			//create a render target
			virtual IRenderTarget* CreateRenderTarget() = 0;
			//obtain a render target by ID
			virtual IRenderTarget* GetRenderTarget(const RenderTargetID rtID) = 0;
			//destroy the render target identified by rtID managed by the manager
			virtual void DestroyRenderTarget(const RenderTargetID rtID) = 0;

			virtual ~IRenderTargetManager() = default;
		};
		using SharedRenderTargetManagerPtr = std::shared_ptr<IRenderTargetManager>;

		using RenderTargetMap = Container::ConcurrentUnorderedMap<RenderTargetID, IRenderTarget*>;
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
				for (auto it = mDestroyedTargetIDs.begin(); it != mDestroyedTargetIDs.end();++it)
				{
					auto renderTarget = mRenderTargets.find(*it);
					if (renderTarget != mRenderTargets.end())
					{
						renderTarget->second->Release();
						mRenderTargets.unsafe_erase(renderTarget);
					}
				}
				mDestroyedTargetIDs.clear();
			}
			//Thread safe
			IRenderTarget* GetRenderTarget(const RenderTargetID targetID) override
			{
				if (mDestroyedTargetIDs.find(targetID) != mDestroyedTargetIDs.end())
					return nullptr;

				auto it = mRenderTargets.find(targetID);
				if (it == mRenderTargets.end())
					return nullptr;

				return it->second;
			}
			//Thread unsafe
			void Clear()
			{
				for (auto it = mRenderTargets.begin(); it != mRenderTargets.end();++it)
				{
					it->second->Release();
				}
				mRenderTargets.clear();
			}

			void DestroyRenderTarget(const RenderTargetID rtID)override
			{
				mDestroyedTargetIDs.insert(rtID);
			}
		protected:
			//Thread safe
			void StoreRenderTarget(RenderTargetID id, IRenderTarget* renderTarget)
			{
				renderTarget->AddRef();
				mRenderTargets[id] = renderTarget;
			}
			RenderTargetMap mRenderTargets;
			Container::ConcurrentUnorderedSet<RenderTargetID> mDestroyedTargetIDs;
		};
	}
}
