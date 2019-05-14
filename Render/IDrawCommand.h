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
		//An IDrawCommand object is an object that encapsulates objects and states that are used to render a drawable object in a frame.
		//It describers what needs to be passed to the downstream rendering pipeline.Users of this interface may change its state by calling 
		//the Set/Get methods whatever they like
		//as long as it is not committed.After the unit is committed,any further operations that potentially
		//change the object is a no-no.Such behavior usually cause undesired outcome.So don't try to reuse it
		//after the commitment.You'd better call Release() after calling IRenderer::CommitDrawCommand
		struct IDrawCommand
		{
			virtual ~IDrawCommand() = default;
			virtual void SetPrimitiveType(PrimitiveType type) = 0;
			virtual void SetIndexBuffer(const std::shared_ptr<IIndexBuffer>& indexBuffer) = 0;
			virtual void ClearVertexBuffers() = 0;
			virtual void SetVertexBuffers(const std::vector<std::shared_ptr<IVertexBuffer>>& vertexBuffers) = 0;
			virtual void SetMaterial(const std::shared_ptr<IMaterial>& material) = 0;
			virtual void SetTransform(const Transform& transform) = 0;
			virtual void SetViewMatrix(const Matrix4f& matrix) = 0;
			virtual void SetProjectionMatrix(const Matrix4f& matrix) = 0;
			virtual void Reset() = 0;
			virtual void Commit() = 0;
			virtual void Release() = 0;
		};
	}
}
