#pragma once
#include "rendererexportdef.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderResourceKeeper
		{
		public:
			//a resource keeper may keep platform dependent rendering resources and those resources can be released by this method.
			virtual void ReleaseRenderResources() = 0;
			virtual ~IRenderResourceKeeper() {}
		};
	}
}