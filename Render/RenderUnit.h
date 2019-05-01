#pragma once
#include <boost/pool/singleton_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <tbb/scalable_allocator.h>
#include "RefObject.h"
#include "IRenderUnit.h"

namespace Lightning
{
	namespace Render
	{
		class RenderUnit : public IRenderUnit
		{
		public:
			RenderUnit();
			~RenderUnit()override;
			void SetPrimitiveType(PrimitiveType type)override;
			PrimitiveType GetPrimitiveType()const override;
			void SetIndexBuffer(IIndexBuffer* indexBuffer)override;
			IIndexBuffer* GetIndexBuffer()const override;
			void ClearVertexBuffers()override;
			void SetVertexBuffer(std::size_t slot, IVertexBuffer* vertexBuffer)override;
			std::size_t GetVertexBufferCount()const override;
			void GetVertexBuffer(std::size_t index, std::size_t& slot, IVertexBuffer*& vertexBuffer)const override;
			void SetMaterial(const std::shared_ptr<IMaterial>& material)override;
			std::shared_ptr<IMaterial> GetMaterial()const override;
			void SetTransform(const Transform& transform)override;
			const Transform& GetTransform()const override;
			void SetViewMatrix(const Matrix4f& matrix)override;
			const Matrix4f& GetViewMatrix()const override;
			void SetProjectionMatrix(const Matrix4f& matrix)override;
			const Matrix4f& GetProjectionMatrix()const override;
			void AddRenderTarget(IRenderTarget* renderTarget)override;
			void RemoveRenderTarget(IRenderTarget* renderTarget)override;
			IRenderTarget* GetRenderTarget(std::size_t index)const override;
			std::size_t GetRenderTargetCount()const override;
			void ClearRenderTargets()override;
			void SetDepthStencilBuffer(IDepthStencilBuffer* depthStencilBuffer)override;
			IDepthStencilBuffer* GetDepthStencilBuffer()const override;
			void Reset()override;
			void AddViewportAndScissorRect(const Viewport& viewport, const ScissorRect& scissorRect)override;
			std::size_t GetViewportCount()const override;
			void GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const override;
			IImmutableRenderUnit * Clone()const override;
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
			void Destroy();
			PrimitiveType mPrimitiveType;
			Transform mTransform;		//position rotation scale
			Matrix4f mViewMatrix;		//camera view matrix
			Matrix4f mProjectionMatrix;//camera projection matrix
			IIndexBuffer* mIndexBuffer;
			std::shared_ptr<IMaterial> mMaterial;	//shader material attributes
			IDepthStencilBuffer* mDepthStencilBuffer; //depth stencil buffer for this draw
			std::vector<IRenderTarget*> mRenderTargets;//render targets
			std::vector<ViewportAndScissorRect> mViewportAndScissorRects;
			std::unordered_map<std::size_t, IVertexBuffer*> mVertexBuffers;
			//std::unordered_map<std::size_t, IVertexBuffer*,
			//	std::hash<std::size_t>, std::equal_to<std::size_t>, VertexBufferAllocatorType> mVertexBuffers;
			bool mCustomRenderTargets;
			bool mCustomDepthStencilBuffer;
			bool mCustomViewport;
			//static VertexBufferAllocatorType sVertexBufferAllocator;
			REF_OBJECT_POOL_OVERRIDE(RenderUnit, Destroy)
		};
		using RenderUnitPool = boost::singleton_pool<RenderUnit, sizeof(RenderUnit)>;
	}
}