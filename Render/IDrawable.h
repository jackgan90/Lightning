#pragma once
#include <memory>
#include <vector>
#include "RenderConstants.h"
#include "IIndexBuffer.h"
#include "IVertexBuffer.h"
#include "IMaterial.h"
#include "Transform.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Transform;

		struct IDrawable
		{
			virtual ~IDrawable() = default;
			virtual PrimitiveType GetPrimitiveType()const = 0;
			virtual std::shared_ptr<IIndexBuffer> GetIndexBuffer()const = 0;
			virtual const std::vector<std::shared_ptr<IVertexBuffer>>& GetVertexBuffers()const = 0;
			virtual std::shared_ptr<IMaterial> GetMaterial()const = 0;
			//This is the global transform
			virtual const Transform GetTransform()const = 0;
		};
	}
}
