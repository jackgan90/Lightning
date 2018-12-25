#pragma once
#include <type_traits>
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
			void Reset()
			{
				primType = PrimitiveType::TRIANGLE_LIST;
				ib = nullptr;
				std::memset(vbs, 0, sizeof(vbs));
			}
			IVertexBuffer* vbs[MAX_GEOMETRY_BUFFER_COUNT];
			IIndexBuffer* ib;
			PrimitiveType primType;
		};
		static_assert(std::is_pod<Geometry>::value, "Geometry is not a POD type.");
	}
}
