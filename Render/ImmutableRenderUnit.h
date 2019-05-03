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
			~ImmutableRenderUnit()override;
			PrimitiveType GetPrimitiveType()const override;
			IIndexBuffer* GetIndexBuffer()const override;
			std::size_t GetVertexBufferCount()const override;
			void GetVertexBuffer(std::size_t index, std::size_t& slot, IVertexBuffer*& vertexBuffer)const override;
			std::shared_ptr<IMaterial> GetMaterial()const override;
			const Transform& GetTransform()const override;
			const Matrix4f& GetViewMatrix()const override;
			const Matrix4f& GetProjectionMatrix()const override;
			std::shared_ptr<IRenderTarget> GetRenderTarget(std::size_t index)const override;
			std::size_t GetRenderTargetCount()const override;
			std::shared_ptr<IDepthStencilBuffer> GetDepthStencilBuffer()const override;
			std::size_t GetViewportCount()const override;
			void GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const override;
			void Commit()override;
		private:
			void CommitBuffers();
			void CommitPipelineStates();
			void CommitShaderParameters();
			void CommitSemanticUniforms(IShader* shader);
			void Draw();
			void GetInputLayouts(std::vector<VertexInputLayout>& inputLayouts);
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
			std::shared_ptr<IMaterial> mMaterial;	//shader material attributes
			std::shared_ptr<IDepthStencilBuffer> mDepthStencilBuffer; //depth stencil buffer for this draw
			std::size_t mViewportCount;
			std::vector<std::shared_ptr<IRenderTarget>> mRenderTargets;
			ViewportAndScissorRect* mViewportAndScissorRects;
			VertexBufferSlot* mVertexBuffers;
			std::size_t mVertexBufferCount;
			REF_OBJECT_POOL_OVERRIDE(ImmutableRenderUnit, Destroy)
		};
		using ImmutableRenderUnitPool = boost::singleton_pool<ImmutableRenderUnit, sizeof(ImmutableRenderUnit)>;
	}
}