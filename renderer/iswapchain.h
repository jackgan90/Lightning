#pragma once
#include "rendererexportdef.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API ISwapChain
		{
		public:
			virtual ~ISwapChain() = 0;
			virtual bool Present() = 0;
		};
	}
}