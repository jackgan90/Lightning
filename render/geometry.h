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
		class LIGHTNINGGE_RENDER_API Geometry
		{
		private:
			SharedVertexBufferPtr m_vbs[MAX_GEOMETRY_BUFFER_COUNT];
			SharedIndexBufferPtr m_ib;
		};
		using SharedGeometryPtr = std::shared_ptr<Geometry>;
	}
}