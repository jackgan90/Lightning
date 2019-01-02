#pragma once
#include <boost/pool/singleton_pool.hpp>
#include "IRenderUnit.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Render
	{
		class ImmutableRenderUnit : public IImmutableRenderUnit
		{
		public:
			ImmutableRenderUnit();
			INTERFACECALL ~ImmutableRenderUnit()override;
			PrimitiveType INTERFACECALL GetPrimitiveType()const override;
			IIndexBuffer* INTERFACECALL GetIndexBuffer()const override;
			std::size_t INTERFACECALL GetVertexBufferCount()const override;
			void INTERFACECALL GetVertexBuffer(std::size_t index, std::size_t& slot, IVertexBuffer*& vertexBuffer)const override;
			IMaterial* INTERFACECALL GetMaterial()const override;
			const Transform& INTERFACECALL GetTransform()const override;
			const Matrix4f& INTERFACECALL GetViewMatrix()const override;
			const Matrix4f& INTERFACECALL GetProjectionMatrix()const override;
			IRenderTarget* INTERFACECALL GetRenderTarget(std::size_t index)const override;
			std::size_t INTERFACECALL GetRenderTargetCount()const override;
			IDepthStencilBuffer* INTERFACECALL GetDepthStencilBuffer()const override;
			std::size_t INTERFACECALL GetViewportCount()const override;
			void INTERFACECALL GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const override;
		private:
			struct ViewportAndScissorRect
			{
				Viewport viewport;
				ScissorRect scissorRect;
			};
			struct VertexBufferSlot
			{
				IVertexBuffer* vertexBuffer;
				std::size_t slot;
			};
			friend class RenderUnit;
			void Destroy();
			PrimitiveType mPrimitiveType;
			Transform mTransform;		//position rotation scale
			Matrix4f mViewMatrix;		//camera view matrix
			Matrix4f mProjectionMatrix;//camera projection matrix
			IIndexBuffer* mIndexBuffer;
			IMaterial* mMaterial;	//shader material attributes
			IDepthStencilBuffer* mDepthStencilBuffer; //depth stencil buffer for this draw
			std::size_t mRenderTargetCount;
			std::size_t mViewportCount;
			IRenderTarget** mRenderTargets;
			ViewportAndScissorRect* mViewportAndScissorRects;
			VertexBufferSlot* mVertexBuffers;
			std::size_t mVertexBufferCount;
			REF_OBJECT_POOL_OVERRIDE(ImmutableRenderUnit, Destroy)
		};
		using ImmutableRenderUnitPool = boost::singleton_pool<ImmutableRenderUnit, sizeof(ImmutableRenderUnit)>;
	}
}