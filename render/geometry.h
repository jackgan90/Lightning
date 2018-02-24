#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "renderconstants.h"

namespace LightningGE
{
	namespace Render
	{
		//a wrapper class for vb/ib 
		struct LIGHTNINGGE_RENDER_API Geometry
		{
			SharedVertexBufferPtr vbs[MAX_GEOMETRY_BUFFER_COUNT];
			SharedIndexBufferPtr ib;
			PrimitiveType primType;
		};
		using SharedGeometryPtr = std::shared_ptr<Geometry>;
	}
}