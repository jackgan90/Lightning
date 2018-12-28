#include <cassert>
#include <algorithm>
#include "Renderer.h"
#include "RenderUnit.h"
#undef min
#undef max

namespace Lightning
{
	namespace Render
	{
		RenderUnit::VertexBufferAllocatorType RenderUnit::sVertexBufferAllocator;
		RenderUnit::RenderUnit()
			: mIndexBuffer(nullptr)
			, mMaterial(nullptr)
			, mDepthStencilBuffer(nullptr)
			, mVertexBuffers(sVertexBufferAllocator)
			, mCustomRenderTargets(false)
			, mCustomDepthStencilBuffer(false)
			, mCustomViewport(false)
		{

		}

		RenderUnit::~RenderUnit()
		{
			DoReset();
		}

		void RenderUnit::SetPrimitiveType(PrimitiveType type)
		{
			mPrimitiveType = type;
		}

		PrimitiveType RenderUnit::GetPrimitiveType()const
		{
			return mPrimitiveType;
		}

		void RenderUnit::SetIndexBuffer(IIndexBuffer* indexBuffer)
		{
			if (mIndexBuffer)
				mIndexBuffer->Release();
			mIndexBuffer = indexBuffer;
			if (mIndexBuffer)
				mIndexBuffer->AddRef();
		}

		IIndexBuffer* RenderUnit::GetIndexBuffer()const
		{
			return mIndexBuffer;
		}

		void RenderUnit::ClearVertexBuffers()
		{
			DoClearVertexBuffers();
		}

		void RenderUnit::SetVertexBuffer(std::size_t slot, IVertexBuffer* vertexBuffer)
		{
			auto it = mVertexBuffers.find(slot);
			if (it != mVertexBuffers.end())
			{
				if (it->second == vertexBuffer)
					return;
				it->second->Release();
				if (vertexBuffer)
				{
					vertexBuffer->AddRef();
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
					vertexBuffer->AddRef();
					mVertexBuffers.emplace(slot, vertexBuffer);
				}
			}
		}

		std::size_t RenderUnit::GetVertexBufferCount()const
		{
			return mVertexBuffers.size();
		}

		void RenderUnit::GetVertexBuffer(std::size_t index, std::size_t& slot, IVertexBuffer*& vertexBuffer)const
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

		void RenderUnit::SetMaterial(IMaterial* material)
		{
			if (material == mMaterial)
				return;
			if (mMaterial)
				mMaterial->Release();
			mMaterial = material;
			if (mMaterial)
				mMaterial->AddRef();
		}

		IMaterial* RenderUnit::GetMaterial()const
		{
			return mMaterial;
		}

		void RenderUnit::SetTransform(const Transform& transform)
		{
			mTransform = transform;
		}

		const Transform& RenderUnit::GetTransform()const
		{
			return mTransform;
		}

		void RenderUnit::SetViewMatrix(const Matrix4f& matrix)
		{
			mViewMatrix = matrix;
		}

		const Matrix4f& RenderUnit::GetViewMatrix()const
		{
			return mViewMatrix;
		}

		void RenderUnit::SetProjectionMatrix(const Matrix4f& matrix)
		{
			mProjectionMatrix = matrix;
		}

		const Matrix4f& RenderUnit::GetProjectionMatrix()const
		{
			return mProjectionMatrix;
		}

		void RenderUnit::AddRenderTarget(IRenderTarget* renderTarget)
		{
			assert(renderTarget != nullptr && "AddRenderTarget only accept valid pointer!");
#ifndef NDEBUG
			auto it = std::find(mRenderTargets.cbegin(), mRenderTargets.cend(), renderTarget);
			assert(it == mRenderTargets.end() && "duplicate render target found.");
#endif
			mCustomRenderTargets = true;
			renderTarget->AddRef();
			mRenderTargets.push_back(renderTarget);
		}

		void RenderUnit::RemoveRenderTarget(IRenderTarget* renderTarget)
		{
			assert(mCustomRenderTargets && "AddRenderTarget must be called prior to call RemoveRenderTarget.");
			auto it = std::find(mRenderTargets.cbegin(), mRenderTargets.cend(), renderTarget);
			if (it != mRenderTargets.end())
			{
				(*it)->Release();
				mRenderTargets.erase(it);
			}
		}

		IRenderTarget* RenderUnit::GetRenderTarget(std::size_t index)const
		{
			if (!mCustomRenderTargets)
			{
				assert(index == 0 && "index exceed container size");
				auto swapChain = Renderer::Instance()->GetSwapChain();
				return swapChain->GetDefaultRenderTarget();
			}
			return mRenderTargets[index];
		}

		std::size_t RenderUnit::GetRenderTargetCount()const
		{
			if (mCustomRenderTargets)
				return mRenderTargets.size();
			else
				return 1u;
		}

		void RenderUnit::ClearRenderTargets()
		{
			DoClearRenderTargets();
		}

		void RenderUnit::SetDepthStencilBuffer(IDepthStencilBuffer* depthStencilBuffer)
		{
			mCustomDepthStencilBuffer = true;
			if (mDepthStencilBuffer == depthStencilBuffer)
				return;
			if (mDepthStencilBuffer)
				mDepthStencilBuffer->Release();
			mDepthStencilBuffer = depthStencilBuffer;
			if (mDepthStencilBuffer)
				mDepthStencilBuffer->AddRef();
		}

		IDepthStencilBuffer* RenderUnit::GetDepthStencilBuffer()const
		{
			if (!mCustomDepthStencilBuffer)
			{
				auto renderer = Renderer::Instance();
				return renderer->GetDefaultDepthStencilBuffer();
			}
			return mDepthStencilBuffer;
		}
		void RenderUnit::AddViewportAndScissorRect(const Viewport& viewport, const ScissorRect& scissorRect)
		{
			mCustomViewport = true;
			mViewportAndScissorRects.push_back({ viewport, scissorRect });
		}

		std::size_t RenderUnit::GetViewportCount()const
		{
			if (!mCustomViewport)
				return std::size_t(1);
			else
				return mViewportAndScissorRects.size();
		}

		void RenderUnit::GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const
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

		void RenderUnit::Reset()
		{
			DoReset();
		}

		IRenderUnit* RenderUnit::Clone()const
		{
			auto clonedUnit = new (RenderUnitPool::malloc()) RenderUnit;
			clonedUnit->mPrimitiveType = mPrimitiveType;
			clonedUnit->mTransform = mTransform;
			clonedUnit->mViewMatrix = mViewMatrix;
			clonedUnit->mProjectionMatrix = mProjectionMatrix;
			clonedUnit->mIndexBuffer = mIndexBuffer;
			clonedUnit->mCustomRenderTargets = mCustomRenderTargets;
			clonedUnit->mCustomDepthStencilBuffer = mCustomDepthStencilBuffer;
			clonedUnit->mCustomViewport = mCustomViewport;
			if (clonedUnit->mIndexBuffer)
			{
				clonedUnit->mIndexBuffer->AddRef();
			}
			clonedUnit->mMaterial = mMaterial;
			if (clonedUnit->mMaterial)
			{
				clonedUnit->mMaterial->AddRef();
			}
			clonedUnit->mDepthStencilBuffer = mDepthStencilBuffer;
			if (clonedUnit->mDepthStencilBuffer)
			{
				clonedUnit->mDepthStencilBuffer->AddRef();
			}
			for (auto renderTarget : mRenderTargets)
			{
				renderTarget->AddRef();
				clonedUnit->mRenderTargets.push_back(renderTarget);
			}
			for (const auto& vs : mViewportAndScissorRects)
			{
				clonedUnit->mViewportAndScissorRects.push_back(vs);
			}
			for (auto it = mVertexBuffers.begin(); it != mVertexBuffers.end();++it)
			{
				it->second->AddRef();
				clonedUnit->mVertexBuffers.emplace(it->first, it->second);
			}
			return clonedUnit;
		}

		void RenderUnit::DoReset()
		{
			if (mIndexBuffer)
			{
				mIndexBuffer->Release();
				mIndexBuffer = nullptr;
			}
			if (mMaterial)
			{
				mMaterial->Release();
				mMaterial = nullptr;
			}
			if (mDepthStencilBuffer)
			{
				mDepthStencilBuffer->Release();
				mDepthStencilBuffer = nullptr;
			}
			DoClearVertexBuffers();
			DoClearRenderTargets();
			DoClearViewportAndScissorRects();
			mCustomDepthStencilBuffer = false;
			mCustomRenderTargets = false;
			mCustomViewport = false;
		}

		void RenderUnit::DoClearRenderTargets()
		{
			for (auto renderTarget : mRenderTargets)
			{
				renderTarget->Release();
			}
			mRenderTargets.clear();
			mCustomRenderTargets = false;
		}

		void RenderUnit::DoClearVertexBuffers()
		{
			for (auto it = mVertexBuffers.begin(); it != mVertexBuffers.end();++it)
			{
				it->second->Release();
			}
			mVertexBuffers.clear();
		}

		void RenderUnit::DoClearViewportAndScissorRects()
		{
			mViewportAndScissorRects.clear();
			mCustomViewport = false;
		}

		void RenderUnit::Destroy()
		{
			this->~RenderUnit();
			RenderUnitPool::free(this);
		}
	}
}