#include <cassert>
#include <algorithm>
#include "Renderer.h"
#include "DrawCall.h"
#include "CommittedDrawCall.h"
#include "FrameMemoryAllocator.h"
#include "Logger.h"
#undef min
#undef max

namespace Lightning
{
	namespace Render
	{
		extern FrameMemoryAllocator g_RenderAllocator;
		DrawCall::DrawCall()
			: mCustomRenderTargets(false)
			, mCustomDepthStencilBuffer(false)
			, mCustomViewport(false)
		{

		}

		DrawCall::~DrawCall()
		{
			DoReset();
		}

		void DrawCall::SetPrimitiveType(PrimitiveType type)
		{
			mPrimitiveType = type;
		}

		PrimitiveType DrawCall::GetPrimitiveType()const
		{
			return mPrimitiveType;
		}

		void DrawCall::SetIndexBuffer(const std::shared_ptr<IIndexBuffer>& indexBuffer)
		{
			mIndexBuffer = indexBuffer;
		}

		std::shared_ptr<IIndexBuffer> DrawCall::GetIndexBuffer()const
		{
			return mIndexBuffer;
		}

		void DrawCall::ClearVertexBuffers()
		{
			DoClearVertexBuffers();
		}

		void DrawCall::SetVertexBuffer(std::size_t slot, const std::shared_ptr<IVertexBuffer>& vertexBuffer)
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

		std::size_t DrawCall::GetVertexBufferCount()const
		{
			return mVertexBuffers.size();
		}

		void DrawCall::GetVertexBuffer(std::size_t index, std::size_t& slot, std::shared_ptr<IVertexBuffer>& vertexBuffer)const
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

		void DrawCall::SetMaterial(const std::shared_ptr<IMaterial>& material)
		{
			if (material == mMaterial)
				return;
			mMaterial = material;
		}

		std::shared_ptr<IMaterial> DrawCall::GetMaterial()const
		{
			return mMaterial;
		}

		void DrawCall::SetTransform(const Transform& transform)
		{
			mTransform = transform;
		}

		const Transform& DrawCall::GetTransform()const
		{
			return mTransform;
		}

		void DrawCall::SetViewMatrix(const Matrix4f& matrix)
		{
			mViewMatrix = matrix;
		}

		const Matrix4f& DrawCall::GetViewMatrix()const
		{
			return mViewMatrix;
		}

		void DrawCall::SetProjectionMatrix(const Matrix4f& matrix)
		{
			mProjectionMatrix = matrix;
		}

		const Matrix4f& DrawCall::GetProjectionMatrix()const
		{
			return mProjectionMatrix;
		}

		void DrawCall::AddRenderTarget(const std::shared_ptr<IRenderTarget>& renderTarget)
		{
			assert(renderTarget != nullptr && "AddRenderTarget only accept valid pointer!");
#ifndef NDEBUG
			auto it = std::find(mRenderTargets.cbegin(), mRenderTargets.cend(), renderTarget);
			assert(it == mRenderTargets.end() && "duplicate render target found.");
#endif
			mCustomRenderTargets = true;
			mRenderTargets.push_back(renderTarget);
		}

		void DrawCall::RemoveRenderTarget(const std::shared_ptr<IRenderTarget>& renderTarget)
		{
			assert(mCustomRenderTargets && "AddRenderTarget must be called prior to call RemoveRenderTarget.");
			auto it = std::find(mRenderTargets.cbegin(), mRenderTargets.cend(), renderTarget);
			if (it != mRenderTargets.end())
			{
				mRenderTargets.erase(it);
			}
		}

		std::shared_ptr<IRenderTarget> DrawCall::GetRenderTarget(std::size_t index)const
		{
			if (!mCustomRenderTargets)
			{
				assert(index == 0 && "index exceed container size");
				auto swapChain = Renderer::Instance()->GetSwapChain();
				return swapChain->GetCurrentRenderTarget();
			}
			return mRenderTargets[index];
		}

		std::size_t DrawCall::GetRenderTargetCount()const
		{
			if (mCustomRenderTargets)
				return mRenderTargets.size();
			else
				return 1u;
		}

		void DrawCall::ClearRenderTargets()
		{
			DoClearRenderTargets();
		}

		void DrawCall::SetDepthStencilBuffer(const std::shared_ptr<IDepthStencilBuffer>& depthStencilBuffer)
		{
			mCustomDepthStencilBuffer = true;
			if (mDepthStencilBuffer == depthStencilBuffer)
				return;
			mDepthStencilBuffer = depthStencilBuffer;
		}

		std::shared_ptr<IDepthStencilBuffer> DrawCall::GetDepthStencilBuffer()const
		{
			if (!mCustomDepthStencilBuffer)
			{
				auto renderer = Renderer::Instance();
				return renderer->GetDefaultDepthStencilBuffer();
			}
			return mDepthStencilBuffer;
		}
		void DrawCall::AddViewportAndScissorRect(const Viewport& viewport, const ScissorRect& scissorRect)
		{
			mCustomViewport = true;
			mViewportAndScissorRects.push_back({ viewport, scissorRect });
		}

		std::size_t DrawCall::GetViewportCount()const
		{
			if (!mCustomViewport)
				return std::size_t(1);
			else
				return mViewportAndScissorRects.size();
		}

		void DrawCall::GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const
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

		void DrawCall::Reset()
		{
			DoReset();
		}

		ICommittedDrawCall* DrawCall::Commit()const
		{
			auto clonedUnit = new (CommittedDrawCallPool::malloc()) CommittedDrawCall;
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
				<CommittedDrawCall::ViewportAndScissorRect>(clonedUnit->mViewportCount);
			for (auto i = 0;i < clonedUnit->mViewportCount;++i)
			{
				GetViewportAndScissorRect(i, clonedUnit->mViewportAndScissorRects[i].viewport
					, clonedUnit->mViewportAndScissorRects[i].scissorRect);
			}

			clonedUnit->mVertexBufferCount = GetVertexBufferCount();
			clonedUnit->mVertexBuffers = g_RenderAllocator.Allocate
				<CommittedDrawCall::VertexBufferSlot>(clonedUnit->mVertexBufferCount);
			for (auto i = 0;i < clonedUnit->mVertexBufferCount;++i)
			{
				GetVertexBuffer(i, clonedUnit->mVertexBuffers[i].slot, 
					clonedUnit->mVertexBuffers[i].vertexBuffer);
			}
			return clonedUnit;
		}

		void DrawCall::DoReset()
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

		void DrawCall::DoClearRenderTargets()
		{
			mRenderTargets.clear();
			mCustomRenderTargets = false;
		}

		void DrawCall::DoClearVertexBuffers()
		{
			mVertexBuffers.clear();
		}

		void DrawCall::DoClearViewportAndScissorRects()
		{
			mViewportAndScissorRects.clear();
			mCustomViewport = false;
		}

		void DrawCall::Release()
		{
			this->~DrawCall();
			DrawCallPool::free(this);
		}

		void DrawCall::Apply()
		{
			//DrawCall cannot apply
			LOG_ERROR("Can't apply the draw call directly!Must invoke Commit and use Apply on CommittedDrawCall!");
			assert(0);
		}
	}
}