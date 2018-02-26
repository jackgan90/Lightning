#include "rendererhelper.h"

namespace LightningGE
{
	namespace Render
	{
		unsigned int GetVertexFormatSize(RenderFormat format)
		{
			switch (format)
			{
			case RenderFormat::R32G32B32_FLOAT:
				return 12;
			default:
				return 0;
			}
		}
	}
}