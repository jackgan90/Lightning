#include "RenderPass.h"
#include "Renderer.h"
#include "DrawCommand.h"

namespace Lightning
{
	namespace Render
	{
		RenderPass::RenderPass() : mCurrentDrawList(&mDrawables[0])
		{

		}

		RenderPass::~RenderPass()
		{
			for (auto i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mDrawables[i].clear();
				for (auto it = mDrawCommands[i].unsafe_begin(); it != mDrawCommands[i].unsafe_end();++it)
				{
					(*it)->Release();
				}
				mDrawCommands[i].clear();
			}
		}

		IDrawCommand* RenderPass::NewDrawCommand()
		{
			auto command = new (DrawCommandPool::malloc()) DrawCommand(*this);
			mDrawCommands[mFrameResourceIndex].emplace(command);
			return command;
		}

		bool RenderPass::AddDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera)
		{
			for (const auto& renderPass : mSubPasses)
			{
				renderPass->AddDrawable(drawable, camera);
			}

			if (AcceptDrawable(drawable, camera))
			{
				mCurrentDrawList->emplace_back(DrawableElement{ drawable, camera });
				return true;
			}
			return false;
		}

		void RenderPass::BeginRender(IRenderer& renderer)
		{
			mFrameResourceIndex = renderer.GetFrameResourceIndex();
			for (auto it = mDrawCommands[mFrameResourceIndex].unsafe_begin(); it != mDrawCommands[mFrameResourceIndex].unsafe_end();++it)
			{
				(*it)->Release();
			}
			mDrawCommands[mFrameResourceIndex].clear();
		}

		void RenderPass::EndRender(IRenderer& renderer)
		{
			for (auto i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				if (mCurrentDrawList == &mDrawables[i])
				{
					auto queueIndex = i + 1;
					if (i == RENDER_FRAME_COUNT - 1)
					{
						queueIndex = 0;
					}
					mCurrentDrawList->clear();
					mCurrentDrawList = &mDrawables[queueIndex];
					break;
				}
			}
		}
	}
}