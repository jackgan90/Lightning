#pragma once
#include <memory>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderConstants.h"

namespace Lightning
{
	namespace Render
	{
		//a wrapper class for vb/ib 
		struct Geometry
		{
			SharedVertexBufferPtr vbs[MAX_GEOMETRY_BUFFER_COUNT];
			SharedIndexBufferPtr ib;
			PrimitiveType primType;
		};
		using SharedGeometryPtr = std::shared_ptr<Geometry>;
	}
}
