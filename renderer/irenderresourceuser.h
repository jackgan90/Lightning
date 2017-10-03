#pragma once
#include "rendererexportdef.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderResourceUser
		{
		public:
			virtual void ReleaseRenderResources() = 0;
			virtual ~IRenderResourceUser() {}
		};
	}
}