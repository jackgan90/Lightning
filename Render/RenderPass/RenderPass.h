#pragma once
#include <memory>
#include <vector>
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_vector.h"
#include "IRenderer.h"
#include "IRenderPass.h"
#include "IDrawCommand.h"

namespace Lightning
{
	namespace Render
	{
		class RenderPass : public IRenderPass
		{
		public:
			RenderPass(IRenderer& renderer);
			~RenderPass()override;
			bool AddDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera)override;
			void BeginRender()override;
			void Render()override;
			void EndRender()override;
			//Render is called by renderer once per frame.Subpasses are also rendered by this method
		protected:
			virtual bool AcceptDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera) = 0;
			virtual void DoRender() = 0;
			IDrawCommand* NewDrawCommand();
			struct DrawableElement
			{
				std::shared_ptr<IDrawable> drawable;
				std::shared_ptr<ICamera> camera;
			};
			tbb::concurrent_vector<DrawableElement> mDrawables[RENDER_FRAME_COUNT];
			tbb::concurrent_queue<IDrawCommand*> mDrawCommands[RENDER_FRAME_COUNT];
			tbb::concurrent_vector<DrawableElement>* mCurrentDrawList;
			std::vector<std::shared_ptr<RenderPass>> mSubPasses;
			std::size_t mFrameResourceIndex;
			IRenderer& mRenderer;
		};
	}
}
