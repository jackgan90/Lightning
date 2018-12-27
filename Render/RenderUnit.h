#pragma once
#include <boost/pool/singleton_pool.hpp>
#include "RefObject.h"
#include "IRenderUnit.h"
#include "Container.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		class RenderUnit : public IRenderUnit
		{
		public:
			RenderUnit();
			INTERFACECALL ~RenderUnit()override;
			void INTERFACECALL SetPrimitiveType(PrimitiveType type)override;
			PrimitiveType INTERFACECALL GetPrimitiveType()const override;
			void INTERFACECALL SetIndexBuffer(IIndexBuffer* indexBuffer)override;
			IIndexBuffer* INTERFACECALL GetIndexBuffer()const override;
			void INTERFACECALL ClearVertexBuffers()override;
			void INTERFACECALL SetVertexBuffer(std::size_t slot, IVertexBuffer* vertexBuffer)override;
			std::size_t INTERFACECALL GetVertexBufferCount()const override;
			void INTERFACECALL GetVertexBuffer(std::size_t index, std::size_t& slot, IVertexBuffer*& vertexBuffer)const override;
			void INTERFACECALL SetMaterial(IMaterial* material)override;
			IMaterial* INTERFACECALL GetMaterial()const override;
			void INTERFACECALL SetTransform(const Transform& transform)override;
			const Transform& INTERFACECALL GetTransform()const override;
			void INTERFACECALL SetViewMatrix(const Matrix4f& matrix)override;
			const Matrix4f& INTERFACECALL GetViewMatrix()const override;
			void INTERFACECALL SetProjectionMatrix(const Matrix4f& matrix)override;
			const Matrix4f& INTERFACECALL GetProjectionMatrix()const override;
			void INTERFACECALL AddRenderTarget(IRenderTarget* renderTarget)override;
			void INTERFACECALL RemoveRenderTarget(IRenderTarget* renderTarget)override;
			IRenderTarget* INTERFACECALL GetRenderTarget(std::size_t index)const override;
			std::size_t INTERFACECALL GetRenderTargetCount()const override;
			void INTERFACECALL ClearRenderTargets()override;
			void INTERFACECALL SetDepthStencilBuffer(IDepthStencilBuffer* depthStencilBuffer)override;
			IDepthStencilBuffer* INTERFACECALL GetDepthStencilBuffer()const override;
			void INTERFACECALL Reset()override;
			IRenderUnit* INTERFACECALL Clone()const override;
		private:
			void DoReset();
			void DoClearRenderTargets();
			void DoClearVertexBuffers();
			//Destroy is only called by object_pool.Never invoke it manually.
			void Destroy();
			PrimitiveType mPrimitiveType;
			Transform mTransform;		//position rotation scale
			Matrix4f mViewMatrix;		//camera view matrix
			Matrix4f mProjectionMatrix;//camera projection matrix
			IIndexBuffer* mIndexBuffer;
			IMaterial* mMaterial;	//shader material attributes
			IDepthStencilBuffer* mDepthStencilBuffer; //depth stencil buffer for this draw
			Container::Vector<IRenderTarget*> mRenderTargets;//render targets
			Container::UnorderedMap<std::size_t, IVertexBuffer*> mVertexBuffers;
			REF_OBJECT_POOL_OVERRIDE(RenderUnit, Destroy)
		};
		using RenderUnitPool = boost::singleton_pool<RenderUnit, sizeof(RenderUnit)>;
	}
}