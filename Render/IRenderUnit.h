#pragma once
#include "Container.h"
#include "Geometry.h"
#include "IMaterial.h"
#include "IRenderTarget.h"
#include "IDepthStencilBuffer.h"
#include "Transform.h"

namespace Lightning
{
	namespace Render
	{
		using namespace Foundation::Math;
		struct IRenderUnit : Plugins::IRefObject
		{
			virtual void INTERFACECALL SetPrimitiveType(PrimitiveType type) = 0;
			virtual PrimitiveType INTERFACECALL GetPrimitiveType()const = 0;
			virtual void INTERFACECALL SetIndexBuffer(IIndexBuffer* indexBuffer) = 0;
			virtual IIndexBuffer* INTERFACECALL GetIndexBuffer()const = 0;
			virtual void INTERFACECALL ClearVertexBuffers() = 0;
			virtual void INTERFACECALL SetVertexBuffer(std::size_t slot, IVertexBuffer* vertexBuffer) = 0;
			virtual std::size_t INTERFACECALL GetVertexBufferCount()const = 0;
			virtual void INTERFACECALL GetVertexBuffer(std::size_t index, std::size_t& slot, IVertexBuffer*& vertexBuffer)const = 0;
			virtual void INTERFACECALL SetMaterial(IMaterial* material) = 0;
			virtual IMaterial* INTERFACECALL GetMaterial()const = 0;
			virtual void INTERFACECALL SetTransform(const Transform& transform) = 0;
			virtual const Transform& INTERFACECALL GetTransform()const = 0;
			virtual void INTERFACECALL SetViewMatrix(const Matrix4f& matrix) = 0;
			virtual const Matrix4f& INTERFACECALL GetViewMatrix()const = 0;
			virtual void INTERFACECALL SetProjectionMatrix(const Matrix4f& matrix) = 0;
			virtual const Matrix4f& INTERFACECALL GetProjectionMatrix()const = 0;
			virtual void INTERFACECALL AddRenderTarget(IRenderTarget* renderTarget) = 0;
			virtual void INTERFACECALL RemoveRenderTarget(IRenderTarget* renderTarget) = 0;
			virtual IRenderTarget* INTERFACECALL GetRenderTarget(std::size_t index)const = 0;
			virtual void INTERFACECALL ClearRenderTargets() = 0;
			virtual std::size_t INTERFACECALL GetRenderTargetCount()const = 0;
			virtual void INTERFACECALL SetDepthStencilBuffer(IDepthStencilBuffer* depthStencilBuffer) = 0;
			virtual IDepthStencilBuffer* INTERFACECALL GetDepthStencilBuffer()const = 0;
			virtual void INTERFACECALL Reset() = 0;
		};

		//RenderQueue is a very important concept.Every drawable object will encode there data to a RenderUnit struct.
		//After enqueue,the nodes remain unchangeable.The responsibility for render pass
		//is to read unit data from the queue and carry out different render algorithms.The data in this
		//queue won't be cleared until next time the renderer renders to the same swap chain,which ensures the 
		//render resource validation during this frame(render resources won't be deleted because a render unit
		//holds all shared_ptr referring to them).
		using RenderQueue = Foundation::Container::Vector<IRenderUnit*>;
	}
}
