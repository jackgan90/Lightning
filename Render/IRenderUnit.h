#pragma once
#include "IMaterial.h"
#include "IRenderTarget.h"
#include "IDepthStencilBuffer.h"
#include "IIndexBuffer.h"
#include "IVertexBuffer.h"
#include "Transform.h"

namespace Lightning
{
	namespace Render
	{
		using namespace Foundation::Math;
		//An immutable render unit.The unit is read-only,used for committed render unit.
		struct IImmutableRenderUnit : Plugins::IRefObject
		{
			virtual PrimitiveType GetPrimitiveType()const = 0;
			virtual IIndexBuffer* GetIndexBuffer()const = 0;
			virtual std::size_t GetVertexBufferCount()const = 0;
			virtual void GetVertexBuffer(std::size_t index, std::size_t& slot, IVertexBuffer*& vertexBuffer)const = 0;
			virtual std::shared_ptr<IMaterial> GetMaterial()const = 0;
			virtual const Transform& GetTransform()const = 0;
			virtual const Matrix4f& GetViewMatrix()const = 0;
			virtual const Matrix4f& GetProjectionMatrix()const = 0;
			virtual IRenderTarget* GetRenderTarget(std::size_t index)const = 0;
			virtual std::size_t GetRenderTargetCount()const = 0;
			virtual IDepthStencilBuffer* GetDepthStencilBuffer()const = 0;
			virtual std::size_t GetViewportCount()const = 0;
			virtual void GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const = 0;
			virtual void Commit() = 0;
		};
		//An IRenderUnit object is an object that encapsulates objects and states that are used to render a drawable object in a frame.
		//It describers what needs to be passed to the downstream rendering pipeline.Users of this interface may change its state by calling 
		//the Set/Get methods whatever they like
		//as long as it is not committed.After the unit is committed,any further operations that potentially
		//change the object is a no-no.Such behavior usually cause undesired outcome.So don't try to reuse it
		//after the commitment.You'd better call Release() after calling IRenderer::CommitRenderUnit.
		struct IRenderUnit : IImmutableRenderUnit
		{
			virtual void SetPrimitiveType(PrimitiveType type) = 0;
			virtual void SetIndexBuffer(IIndexBuffer* indexBuffer) = 0;
			virtual void ClearVertexBuffers() = 0;
			virtual void SetVertexBuffer(std::size_t slot, IVertexBuffer* vertexBuffer) = 0;
			virtual void SetMaterial(const std::shared_ptr<IMaterial>& material) = 0;
			virtual void SetTransform(const Transform& transform) = 0;
			virtual void SetViewMatrix(const Matrix4f& matrix) = 0;
			virtual void SetProjectionMatrix(const Matrix4f& matrix) = 0;
			virtual void AddRenderTarget(IRenderTarget* renderTarget) = 0;
			virtual void RemoveRenderTarget(IRenderTarget* renderTarget) = 0;
			virtual void ClearRenderTargets() = 0;
			virtual void SetDepthStencilBuffer(IDepthStencilBuffer* depthStencilBuffer) = 0;
			//Viewport and scissorRect must be set atomic
			virtual void AddViewportAndScissorRect(const Viewport& viewport, const ScissorRect& scissorRect) = 0;
			virtual void Reset() = 0;
			virtual IImmutableRenderUnit* Clone()const = 0;
		};

		//RenderQueue is a very important concept.Every drawable object will encode there data to a RenderUnit struct.
		//After enqueue,the nodes remain unchangeable.The responsibility for render pass
		//is to read unit data from the queue and carry out different render algorithms.The data in this
		//queue won't be cleared until next time the renderer renders to the same swap chain,which ensures the 
		//render resource validation during this frame(render resources won't be deleted because a render unit
		//holds all shared_ptr referring to them).
		using RenderQueue = std::vector<IImmutableRenderUnit*>;
	}
}
