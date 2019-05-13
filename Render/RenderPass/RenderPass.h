#pragma once
#include <memory>
#include <vector>
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_vector.h"
#include "IRenderPass.h"
#include "IDrawCommand.h"
#include "IRenderTarget.h"

namespace Lightning
{
	namespace Render
	{
		class RenderPass : public IRenderPass
		{
		public:
			RenderPass();
			~RenderPass()override;
			bool AddDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera)override;
			void BeginRender(IRenderer& renderer)override;
			void EndRender(IRenderer& renderer)override;
		protected:
			virtual bool AcceptDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera) = 0;
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
		};
	}
}
