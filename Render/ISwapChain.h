#pragma once
#include <memory>
#include "IRenderTarget.h"

namespace Lightning
{
	namespace Render
	{
		struct ISwapChain
		{
			virtual ~ISwapChain() = default;
			//preset the current back buffer to display
			virtual bool Present() = 0;
			virtual void Resize(std::size_t width, std::size_t height) = 0;
			virtual std::size_t GetMultiSampleCount()const = 0;
			virtual std::size_t GetMultiSampleQuality()const = 0;
			virtual RenderFormat GetRenderFormat()const = 0;
			//get corresponding render target
			virtual std::shared_ptr<IRenderTarget> GetCurrentRenderTarget() = 0;
		};
	}
}
