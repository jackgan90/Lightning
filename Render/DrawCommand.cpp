#include <cassert>
#include <algorithm>
#include "Renderer.h"
#include "DrawCommand.h"
#include "CommittedDrawCommand.h"
#include "FrameMemoryAllocator.h"
#include "Logger.h"
#undef min
#undef max

namespace Lightning
{
	namespace Render
	{
		extern FrameMemoryAllocator g_RenderAllocator;
		DrawCommand::DrawCommand()
			: mCustomRenderTargets(false)
			, mCustomDepthStencilBuffer(false)
			, mCustomViewport(false)
		{

		}

		DrawCommand::~DrawCommand()
		{
			DoReset();
		}

		void DrawCommand::SetPrimitiveType(PrimitiveType type)
		{
			mPrimitiveType = type;
		}

		PrimitiveType DrawCommand::GetPrimitiveType()const
		{
			return mPrimitiveType;
		}

		void DrawCommand::SetIndexBuffer(const std::shared_ptr<IIndexBuffer>& indexBuffer)
		{
			mIndexBuffer = indexBuffer;
		}

		std::shared_ptr<IIndexBuffer> DrawCommand::GetIndexBuffer()const
		{
			return mIndexBuffer;
		}

		void DrawCommand::ClearVertexBuffers()
		{
			DoClearVertexBuffers();
		}

		void DrawCommand::SetVertexBuffer(std::size_t slot, const std::shared_ptr<IVertexBuffer>& vertexBuffer)
		{
			auto it = mVertexBuffers.find(slot);
			if (it != mVertexBuffers.end())
			{
				if (it->second == vertexBuffer)
					return;
				if (vertexBuffer)
				{
					it->second = vertexBuffer;
				}
				else
				{
					mVertexBuffers.erase(it);
				}
			}
			else
			{
				if (vertexBuffer)
				{
					mVertexBuffers.emplace(slot, vertexBuffer);
				}
			}
		}

		std::size_t DrawCommand::GetVertexBufferCount()const
		{
			return mVertexBuffers.size();
		}

		void DrawCommand::GetVertexBuffer(std::size_t index, std::size_t& slot, std::shared_ptr<IVertexBuffer>& vertexBuffer)const
		{
			assert(index < mVertexBuffers.size() && "index out of range.");
			auto i = 0;
			for (auto it = mVertexBuffers.begin(); it != mVertexBuffers.end();++it, ++i)
			{
				if (i == index)
				{
					slot = it->first;
					vertexBuffer = it->second;
					break;
				}
			}
		}

		void DrawCommand::SetMaterial(const std::shared_ptr<IMaterial>& material)
		{
			if (material == mMaterial)
				return;
			mMaterial = material;
		}

		std::shared_ptr<IMaterial> DrawCommand::GetMaterial()const
		{
			return mMaterial;
		}

		void DrawCommand::SetTransform(const Transform& transform)
		{
			mTransform = transform;
		}

		const Transform& DrawCommand::GetTransform()const
		{
			return mTransform;
		}

		void DrawCommand::SetViewMatrix(const Matrix4f& matrix)
		{
			mViewMatrix = matrix;
		}

		const Matrix4f& DrawCommand::GetViewMatrix()const
		{
			return mViewMatrix;
		}

		void DrawCommand::SetProjectionMatrix(const Matrix4f& matrix)
		{
			mProjectionMatrix = matrix;
		}

		const Matrix4f& DrawCommand::GetProjectionMatrix()const
		{
			return mProjectionMatrix;
		}

		void DrawCommand::AddRenderTarget(const std::shared_ptr<IRenderTarget>& renderTarget)
		{
			assert(renderTarget != nullptr && "AddRenderTarget only accept valid pointer!");
#ifndef NDEBUG
			auto it = std::find(mRenderTargets.cbegin(), mRenderTargets.cend(), renderTarget);
			assert(it == mRenderTargets.end() && "duplicate render target found.");
#endif
			mCustomRenderTargets = true;
			mRenderTargets.push_back(renderTarget);
		}

		void DrawCommand::RemoveRenderTarget(const std::shared_ptr<IRenderTarget>& renderTarget)
		{
			assert(mCustomRenderTargets && "AddRenderTarget must be called prior to call RemoveRenderTarget.");
			auto it = std::find(mRenderTargets.cbegin(), mRenderTargets.cend(), renderTarget);
			if (it != mRenderTargets.end())
			{
				mRenderTargets.erase(it);
			}
		}

		std::shared_ptr<IRenderTarget> DrawCommand::GetRenderTarget(std::size_t index)const
		{
			if (!mCustomRenderTargets)
			{
				assert(index == 0 && "index exceed container size");
				auto swapChain = Renderer::Instance()->GetSwapChain();
				return swapChain->GetCurrentRenderTarget();
			}
			return mRenderTargets[index];
		}

		std::size_t DrawCommand::GetRenderTargetCount()const
		{
			if (mCustomRenderTargets)
				return mRenderTargets.size();
			else
				return 1u;
		}

		void DrawCommand::ClearRenderTargets()
		{
			DoClearRenderTargets();
		}

		void DrawCommand::SetDepthStencilBuffer(const std::shared_ptr<IDepthStencilBuffer>& depthStencilBuffer)
		{
			mCustomDepthStencilBuffer = true;
			if (mDepthStencilBuffer == depthStencilBuffer)
				return;
			mDepthStencilBuffer = depthStencilBuffer;
		}

		std::shared_ptr<IDepthStencilBuffer> DrawCommand::GetDepthStencilBuffer()const
		{
			if (!mCustomDepthStencilBuffer)
			{
				auto renderer = Renderer::Instance();
				return renderer->GetDefaultDepthStencilBuffer();
			}
			return mDepthStencilBuffer;
		}
		void DrawCommand::AddViewportAndScissorRect(const Viewport& viewport, const ScissorRect& scissorRect)
		{
			mCustomViewport = true;
			mViewportAndScissorRects.push_back({ viewport, scissorRect });
		}

		std::size_t DrawCommand::GetViewportCount()const
		{
			if (!mCustomViewport)
				return std::size_t(1);
			else
				return mViewportAndScissorRects.size();
		}

		void DrawCommand::GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const
		{
			if (!mCustomViewport)
			{
				assert(index == 0 && "Default viewport and scissorRect count is 1.");
				auto renderer = Renderer::Instance();
				auto window = renderer->GetOutputWindow();
				viewport.left = viewport.top = .0f;
				viewport.width = static_cast<float>(window->GetWidth());
				viewport.height = static_cast<float>(window->GetHeight());

				scissorRect.left = static_cast<long>(viewport.left);
				scissorRect.top = static_cast<long>(viewport.top);
				scissorRect.width = static_cast<long>(viewport.width);
				scissorRect.height = static_cast<long>(viewport.height);
			}
			else
			{
				assert(index < mViewportAndScissorRects.size() && "viewport index out of range.");
				const auto& vs = mViewportAndScissorRects[index];
				viewport = vs.viewport;
				scissorRect = vs.scissorRect;
			}
		}

		void DrawCommand::Reset()
		{
			DoReset();
		}

		ICommittedDrawCommand* DrawCommand::Commit()const
		{
			auto clonedUnit = new (CommittedDrawCommandPool::malloc()) CommittedDrawCommand;
			clonedUnit->mPrimitiveType = GetPrimitiveType();
			clonedUnit->mTransform = GetTransform();
			clonedUnit->mViewMatrix = GetViewMatrix();
			clonedUnit->mProjectionMatrix = GetProjectionMatrix();

			clonedUnit->mIndexBuffer = GetIndexBuffer();

			clonedUnit->mMaterial = GetMaterial();

			clonedUnit->mDepthStencilBuffer = GetDepthStencilBuffer();
			for (auto i = 0;i < GetRenderTargetCount();++i)
			{
				clonedUnit->mRenderTargets.push_back(GetRenderTarget(i));
			}


			clonedUnit->mViewportCount = GetViewportCount();
			clonedUnit->mViewportAndScissorRects = g_RenderAllocator.Allocate 
				<CommittedDrawCommand::ViewportAndScissorRect>(clonedUnit->mViewportCount);
			for (auto i = 0;i < clonedUnit->mViewportCount;++i)
			{
				GetViewportAndScissorRect(i, clonedUnit->mViewportAndScissorRects[i].viewport
					, clonedUnit->mViewportAndScissorRects[i].scissorRect);
			}

			clonedUnit->mVertexBufferCount = GetVertexBufferCount();
			clonedUnit->mVertexBuffers = g_RenderAllocator.Allocate
				<CommittedDrawCommand::VertexBufferSlot>(clonedUnit->mVertexBufferCount);
			for (auto i = 0;i < clonedUnit->mVertexBufferCount;++i)
			{
				GetVertexBuffer(i, clonedUnit->mVertexBuffers[i].slot, 
					clonedUnit->mVertexBuffers[i].vertexBuffer);
			}
			return clonedUnit;
		}

		void DrawCommand::DoReset()
		{
			mIndexBuffer.reset();
			mMaterial.reset();
			mDepthStencilBuffer.reset();
			DoClearVertexBuffers();
			DoClearRenderTargets();
			DoClearViewportAndScissorRects();
			mCustomDepthStencilBuffer = false;
			mCustomRenderTargets = false;
			mCustomViewport = false;
		}

		void DrawCommand::DoClearRenderTargets()
		{
			mRenderTargets.clear();
			mCustomRenderTargets = false;
		}

		void DrawCommand::DoClearVertexBuffers()
		{
			mVertexBuffers.clear();
		}

		void DrawCommand::DoClearViewportAndScissorRects()
		{
			mViewportAndScissorRects.clear();
			mCustomViewport = false;
		}

		void DrawCommand::Release()
		{
			this->~DrawCommand();
			DrawCommandPool::free(this);
		}

		void DrawCommand::Apply()
		{
			//DrawCommand cannot apply
			LOG_ERROR("Can't apply the draw call directly!Must invoke Commit and use Apply on CommittedDrawCommand!");
			assert(0);
		}
	}
}