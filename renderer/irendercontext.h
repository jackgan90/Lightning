#pragma once
#include "rendererexportdef.h"
#include "iwindow.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IRenderContext
		{
		public:
			virtual ~IRenderContext() = 0;
			virtual bool Init(WindowPtr pWindow) = 0;
		};
	}
}
