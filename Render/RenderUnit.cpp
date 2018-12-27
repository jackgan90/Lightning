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
		RenderUnit::RenderUnit()
			: mIndexBuffer(nullptr)
			, mMaterial(nullptr)
			, mDepthStencilBuffer(nullptr)
			, mCustomRenderTargets(false)
			, mCustomDepthStencilBuffer(false)
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
			mCustomDepthStencilBuffer = false;
			mCustomRenderTargets = false;
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

		void RenderUnit::Destroy()
		{
			this->~RenderUnit();
			RenderUnitPool::free(this);
		}
	}
}