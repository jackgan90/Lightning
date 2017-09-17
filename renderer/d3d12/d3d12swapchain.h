#pragma once
#include "iswapchain.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API D3D12SwapChain : ISwapChain
		{
		public:
			~D3D12SwapChain()override;
			bool Present()override;
		};
	}
}