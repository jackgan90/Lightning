#include <sstream>
#include "RendererHelper.h"

namespace Lightning
{
	namespace Render
	{
		unsigned int GetVertexFormatSize(RenderFormat format)
		{
			switch (format)
			{
			case RenderFormat::R32G32_FLOAT:
				return 8;
			case RenderFormat::R32G32B32_FLOAT:
				return 12;
			case RenderFormat::R32G32B32A32_FLOAT:
				return 16;
			default:
				return 0;
			}
		}

	}
}