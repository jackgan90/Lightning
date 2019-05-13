#include "RenderPass.h"
#include "Renderer.h"
#include "DrawCommand.h"

namespace Lightning
{
	namespace Render
	{
		RenderPass::RenderPass(IRenderer& renderer) 
			: mCurrentDrawList(&mDrawables[0])
			, mRenderer(renderer)
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
			auto command = new (DrawCommandPool::malloc()) DrawCommand(mRenderer, *this);
			mDrawCommands[mFrameResourceIndex].emplace(command);
			return command;
		}

		void RenderPass::Render()
		{
			DoRender();
			for (const auto& renderPass : mSubPasses)
			{
				renderPass->Render();
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

		void RenderPass::BeginRender()
		{
			mFrameResourceIndex = mRenderer.GetFrameResourceIndex();
			for (auto it = mDrawCommands[mFrameResourceIndex].unsafe_begin(); it != mDrawCommands[mFrameResourceIndex].unsafe_end();++it)
			{
				(*it)->Release();
			}
			mDrawCommands[mFrameResourceIndex].clear();

			for (const auto& renderPass : mSubPasses)
			{
				renderPass->BeginRender();
			}

		}

		void RenderPass::EndRender()
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
				renderPass->EndRender();
			}
		}
	}
}