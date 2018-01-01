#include "rendererhelper.h"

namespace LightningGE
{
	namespace Render
	{
		unsigned int GetVertexFormatSize(VertexFormat format)
		{
			switch (format)
			{
			case VERTEX_FORMAT_R32G32B32_FLOAT:
				return 12;
			default:
				return 0;
			}
		}
	}
}