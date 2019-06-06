#pragma once
#include <boost/pool/singleton_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <tbb/scalable_allocator.h>
#include "IRenderer.h"
#include "IDrawCommand.h"
#include "RenderPass/IRenderPass.h"

namespace Lightning
{
	namespace Render
	{
		class DrawCommand : public IDrawCommand
		{
		public:
			DrawCommand(IRenderer& renderer, IRenderPass& renderPass);
			~DrawCommand()override;
			void SetPrimitiveType(PrimitiveType type)override;
			void SetIndexBuffer(const std::shared_ptr<IIndexBuffer>& indexBuffer)override;
			void ClearVertexBuffers()override;
			void SetVertexBuffers(const std::vector<std::shared_ptr<IVertexBuffer>>& vertexBuffers)override;
			void SetMaterial(const std::shared_ptr<IMaterial>& material)override;
			void SetTransform(const Transform& transform)override;
			void SetViewMatrix(const Matrix4f& matrix)override;
			void SetProjectionMatrix(const Matrix4f& matrix)override;
			void Reset()override;
			void Commit()override;
			void Release()override;
		private:
			void DoReset();
			void DoClearVertexBuffers();
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
			std::vector<std::shared_ptr<IVertexBuffer>> mVertexBuffers;
			IRenderPass& mRenderPass;
			IRenderer& mRenderer;
		};
		using DrawCommandPool = boost::singleton_pool<DrawCommand, sizeof(DrawCommand)>;
	}
}