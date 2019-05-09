#pragma once
#include <boost/pool/singleton_pool.hpp>
#include "IDrawCall.h"

namespace Lightning
{
	namespace Render
	{
		class CommittedDrawCall : public ICommittedDrawCall
		{
		public:
			CommittedDrawCall();
			~CommittedDrawCall()override;
			PrimitiveType GetPrimitiveType()const override;
			std::shared_ptr<IIndexBuffer> GetIndexBuffer()const override;
			std::size_t GetVertexBufferCount()const override;
			void GetVertexBuffer(std::size_t index, std::size_t& slot, std::shared_ptr<IVertexBuffer>& vertexBuffer)const override;
			std::shared_ptr<IMaterial> GetMaterial()const override;
			const Transform& GetTransform()const override;
			const Matrix4f& GetViewMatrix()const override;
			const Matrix4f& GetProjectionMatrix()const override;
			std::shared_ptr<IRenderTarget> GetRenderTarget(std::size_t index)const override;
			std::size_t GetRenderTargetCount()const override;
			std::shared_ptr<IDepthStencilBuffer> GetDepthStencilBuffer()const override;
			std::size_t GetViewportCount()const override;
			void GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const override;
			void Apply()override;
			void Release()override;
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
				std::shared_ptr<IVertexBuffer> vertexBuffer;
				std::size_t slot;
			};
			friend class DrawCall;
			PrimitiveType mPrimitiveType;
			Transform mTransform;		//position rotation scale
			Matrix4f mViewMatrix;		//camera view matrix
			Matrix4f mProjectionMatrix;//camera projection matrix
			std::shared_ptr<IIndexBuffer> mIndexBuffer;
			std::shared_ptr<IMaterial> mMaterial;	//shader material attributes
			std::shared_ptr<IDepthStencilBuffer> mDepthStencilBuffer; //depth stencil buffer for this draw
			std::size_t mViewportCount;
			std::vector<std::shared_ptr<IRenderTarget>> mRenderTargets;
			ViewportAndScissorRect* mViewportAndScissorRects;
			VertexBufferSlot* mVertexBuffers;
			std::size_t mVertexBufferCount;
		};
		using CommittedDrawCallPool = boost::singleton_pool<CommittedDrawCall, sizeof(CommittedDrawCall)>;
	}
}