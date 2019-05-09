#pragma once
#include <boost/pool/singleton_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <tbb/scalable_allocator.h>
#include "RefObject.h"
#include "IDrawCommand.h"

namespace Lightning
{
	namespace Render
	{
		class DrawCommand : public IDrawCommand
		{
		public:
			DrawCommand();
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
			void AddRenderTarget(const std::shared_ptr<IRenderTarget>& renderTarget)override;
			void RemoveRenderTarget(const std::shared_ptr<IRenderTarget>& renderTarget)override;
			std::shared_ptr<IRenderTarget> GetRenderTarget(std::size_t index)const override;
			std::size_t GetRenderTargetCount()const override;
			void ClearRenderTargets()override;
			void SetDepthStencilBuffer(const std::shared_ptr<IDepthStencilBuffer>& depthStencilBuffer)override;
			std::shared_ptr<IDepthStencilBuffer> GetDepthStencilBuffer()const override;
			void Reset()override;
			void AddViewportAndScissorRect(const Viewport& viewport, const ScissorRect& scissorRect)override;
			std::size_t GetViewportCount()const override;
			void GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const override;
			ICommittedDrawCommand * Commit()const override;
			void Apply()override;
			void Release()override;
		private:
			//using VertexBufferAllocatorType = boost::pool_allocator<std::pair<const std::size_t, IVertexBuffer*>>;
			//using VertexBufferAllocatorType = std::allocator<std::pair<const std::size_t, IVertexBuffer*>>;
			//using VertexBufferAllocatorType = tbb::scalable_allocator<std::pair<const std::size_t, IVertexBuffer*>>;
			struct ViewportAndScissorRect
			{
				Viewport viewport;
				ScissorRect scissorRect;
			};
			void DoReset();
			void DoClearRenderTargets();
			void DoClearVertexBuffers();
			void DoClearViewportAndScissorRects();
			//Destroy is only called by object_pool.Never invoke it manually.
			PrimitiveType mPrimitiveType;
			Transform mTransform;		//position rotation scale
			Matrix4f mViewMatrix;		//camera view matrix
			Matrix4f mProjectionMatrix;//camera projection matrix
			std::shared_ptr<IIndexBuffer> mIndexBuffer;
			std::shared_ptr<IMaterial> mMaterial;	//shader material attributes
			std::shared_ptr<IDepthStencilBuffer> mDepthStencilBuffer; //depth stencil buffer for this draw
			std::vector<std::shared_ptr<IRenderTarget>> mRenderTargets;//render targets
			std::vector<ViewportAndScissorRect> mViewportAndScissorRects;
			std::unordered_map<std::size_t, std::shared_ptr<IVertexBuffer>> mVertexBuffers;
			//std::unordered_map<std::size_t, IVertexBuffer*,
			//	std::hash<std::size_t>, std::equal_to<std::size_t>, VertexBufferAllocatorType> mVertexBuffers;
			bool mCustomRenderTargets;
			bool mCustomDepthStencilBuffer;
			bool mCustomViewport;
			//static VertexBufferAllocatorType sVertexBufferAllocator;
		};
		using DrawCommandPool = boost::singleton_pool<DrawCommand, sizeof(DrawCommand)>;
	}
}