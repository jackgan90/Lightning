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
			IVertexBuffer* vbs[MAX_GEOMETRY_BUFFER_COUNT];
			IIndexBuffer* ib;
			PrimitiveType primType;
		};
	}
}
