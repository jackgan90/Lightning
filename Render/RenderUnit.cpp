#include <cassert>
#include <algorithm>
#include <boost/pool/object_pool.hpp>
#include "RenderUnit.h"

namespace Lightning
{
	namespace Render
	{
		extern thread_local boost::object_pool<RenderUnit> g_RenderUnitPool;
		RenderUnit::RenderUnit():mIndexBuffer(nullptr), mMaterial(nullptr), mDepthStencilBuffer(nullptr)
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
			renderTarget->AddRef();
			mRenderTargets.push_back(renderTarget);
		}

		void RenderUnit::RemoveRenderTarget(IRenderTarget* renderTarget)
		{
			auto it = std::find(mRenderTargets.cbegin(), mRenderTargets.cend(), renderTarget);
			if (it != mRenderTargets.end())
			{
				(*it)->Release();
				mRenderTargets.erase(it);
			}
		}

		IRenderTarget* RenderUnit::GetRenderTarget(std::size_t index)const
		{
			return mRenderTargets[index];
		}

		std::size_t RenderUnit::GetRenderTargetCount()const
		{
			return mRenderTargets.size();
		}

		void RenderUnit::ClearRenderTargets()
		{
			DoClearRenderTargets();
		}

		void RenderUnit::SetDepthStencilBuffer(IDepthStencilBuffer* depthStencilBuffer)
		{
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
			return mDepthStencilBuffer;
		}

		void RenderUnit::Reset()
		{
			DoReset();
		}

		IRenderUnit* RenderUnit::Clone()const
		{
			auto clonedUnit = g_RenderUnitPool.construct();
			clonedUnit->mPrimitiveType = mPrimitiveType;
			clonedUnit->mTransform = mTransform;
			clonedUnit->mViewMatrix = mViewMatrix;
			clonedUnit->mProjectionMatrix = mProjectionMatrix;
			clonedUnit->mIndexBuffer = mIndexBuffer;
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
		}

		void RenderUnit::DoClearRenderTargets()
		{
			for (auto renderTarget : mRenderTargets)
			{
				renderTarget->Release();
			}
			mRenderTargets.clear();
		}

		void RenderUnit::DoClearVertexBuffers()
		{
			for (auto it = mVertexBuffers.begin(); it != mVertexBuffers.end();++it)
			{
				it->second->Release();
			}
			mVertexBuffers.clear();
		}
	}
}