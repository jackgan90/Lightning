#include "ImmutableRenderUnit.h"

namespace Lightning
{
	namespace Render
	{
		ImmutableRenderUnit::ImmutableRenderUnit()
		{

		}

		ImmutableRenderUnit::~ImmutableRenderUnit()
		{
			if (mIndexBuffer)
			{
				mIndexBuffer->Release();
			}
			if (mDepthStencilBuffer)
			{
				mDepthStencilBuffer->Release();
			}
			for (auto i = 0;i < mRenderTargetCount;++i)
			{
				mRenderTargets[i]->Release();
			}
			for (auto i = 0;i < mVertexBufferCount;++i)
			{
				mVertexBuffers[i].vertexBuffer->Release();
			}
		}

		PrimitiveType ImmutableRenderUnit::GetPrimitiveType()const
		{
			return mPrimitiveType;
		}

		IIndexBuffer* ImmutableRenderUnit::GetIndexBuffer()const
		{
			return mIndexBuffer;
		}

		std::size_t ImmutableRenderUnit::GetVertexBufferCount()const
		{
			return mVertexBufferCount;
		}

		void ImmutableRenderUnit::GetVertexBuffer(std::size_t index, std::size_t& slot, IVertexBuffer*& vertexBuffer)const
		{
			slot = mVertexBuffers[index].slot;
			vertexBuffer = mVertexBuffers[index].vertexBuffer;
		}

		std::shared_ptr<IMaterial> ImmutableRenderUnit::GetMaterial()const
		{
			return mMaterial;
		}

		const Transform& ImmutableRenderUnit::GetTransform()const
		{
			return mTransform;
		}

		const Matrix4f& ImmutableRenderUnit::GetViewMatrix()const
		{
			return mViewMatrix;
		}

		const Matrix4f& ImmutableRenderUnit::GetProjectionMatrix()const
		{
			return mProjectionMatrix;
		}

		IRenderTarget* ImmutableRenderUnit::GetRenderTarget(std::size_t index)const
		{
			return mRenderTargets[index];
		}

		std::size_t ImmutableRenderUnit::GetRenderTargetCount()const
		{
			return mRenderTargetCount;
		}

		IDepthStencilBuffer* ImmutableRenderUnit::GetDepthStencilBuffer()const
		{
			return mDepthStencilBuffer;
		}

		std::size_t ImmutableRenderUnit::GetViewportCount()const
		{
			return mViewportCount;
		}

		void ImmutableRenderUnit::GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const
		{
			viewport = mViewportAndScissorRects[index].viewport;
			scissorRect = mViewportAndScissorRects[index].scissorRect;
		}

		void ImmutableRenderUnit::Destroy()
		{
			this->~ImmutableRenderUnit();
			ImmutableRenderUnitPool::free(this);
		}
	}
}