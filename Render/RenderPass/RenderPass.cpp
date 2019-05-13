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

		void RenderPass::Render(IRenderer& renderer)
		{
			DoRender(renderer);
			for (const auto& renderPass : mSubPasses)
			{
				renderPass->Render(renderer);
			}
		}

		bool RenderPass::AddDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera)
		{
			bool succeed{ false };
			if (AcceptDrawable(drawable, camera))
			{
				mCurrentDrawList->emplace_back(DrawableElement{ drawable, camera });
				succeed = true;
			}

			for (const auto& renderPass : mSubPasses)
			{
				renderPass->AddDrawable(drawable, camera);
			}
			return succeed;
		}

		void RenderPass::BeginRender(IRenderer& renderer)
		{
			mFrameResourceIndex = renderer.GetFrameResourceIndex();
			for (auto it = mDrawCommands[mFrameResourceIndex].unsafe_begin(); it != mDrawCommands[mFrameResourceIndex].unsafe_end();++it)
			{
				(*it)->Release();
			}
			mDrawCommands[mFrameResourceIndex].clear();

			for (const auto& renderPass : mSubPasses)
			{
				renderPass->BeginRender(renderer);
			}

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

			for (const auto& renderPass : mSubPasses)
			{
				renderPass->EndRender(renderer);
			}
		}
	}
}