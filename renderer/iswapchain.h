#pragma once
#include "rendererexportdef.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API ISwapChain
		{
		public:
			virtual ~ISwapChain() {};
			virtual bool Present() = 0;
			virtual unsigned int GetBufferCount()const = 0;
		};
		using SwapChainPtr = std::shared_ptr<ISwapChain>;
	}
}