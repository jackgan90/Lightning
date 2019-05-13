#pragma once
#include <boost/pool/singleton_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <tbb/scalable_allocator.h>
#include "RefObject.h"
#include "IDrawCommand.h"
#include "RenderPass/IRenderPass.h"

namespace Lightning
{
	namespace Render
	{
		class DrawCommand : public IDrawCommand
		{
		public:
			DrawCommand(IRenderPass& renderPass);
			~DrawCommand()override;
			void SetPrimitiveType(PrimitiveType type)override;
			PrimitiveType GetPrimitiveType()const override;
			void SetIndexBuffer(const std::shared_ptr<IIndexBuffer>& indexBuffer)override;
			std::shared_ptr<IIndexBuffer> GetIndexBuffer()const override;
			void ClearVertexBuffers()override;
			void SetVertexBuffer(std::size_t slot, const std::shared_ptr<IVertexBuffer>& vertexBuffer)override;
			std::size_t GetVertexBufferCount()const override;
			void GetVertexBuffer(std::size_t index, std::size_t& slot, std::shared_ptr<IVertexBuffer>& vertexBuffer)const override;
			void SetMaterial(const std::shared_ptr<IMaterial>& material)override;
			std::shared_ptr<IMaterial> GetMaterial()const override;
			void SetTransform(const Transform& transform)override;
			const Transform& GetTransform()const override;
			void SetViewMatrix(const Matrix4f& matrix)override;
			const Matrix4f& GetViewMatrix()const override;
			void SetProjectionMatrix(const Matrix4f& matrix)override;
			const Matrix4f& GetProjectionMatrix()const override;
			void Reset()override;
			void Commit()override;
			void Release()override;
		private:
			struct ViewportAndScissorRect
			{
				Viewport viewport;
				ScissorRect scissorRect;
			};
			void DoReset();
			//void DoClearRenderTargets();
			void DoClearVertexBuffers();
			//void DoClearViewportAndScissorRects();
			void CommitBuffers();
			void CommitPipelineStates();
			void CommitShaderParameters();
			void CommitSemanticUniforms(IShader* shader);
			void Draw();
			void GetInputLayouts(std::vector<VertexInputLayout>& inputLayouts);
			PrimitiveType mPrimitiveType;
			Transform mTransform;		//position rotation scale
			Matrix4f mViewMatrix;		//camera view matrix
			Matrix4f mProjectionMatrix;//camera projection matrix
			std::shared_ptr<IIndexBuffer> mIndexBuffer;
			std::shared_ptr<IMaterial> mMaterial;	//shader material attributes
			std::unordered_map<std::size_t, std::shared_ptr<IVertexBuffer>> mVertexBuffers;
			IRenderPass& mRenderPass;
		};
		using DrawCommandPool = boost::singleton_pool<DrawCommand, sizeof(DrawCommand)>;
	}
}