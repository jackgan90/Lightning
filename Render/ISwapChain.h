#pragma once
#include <memory>
#include <exception>
#include "IRenderTarget.h"

namespace Lightning
{
	namespace Render
	{
		class ISwapChain
		{
		public:
			virtual ~ISwapChain() = default;
			//preset the current back buffer to display
			virtual bool Present() = 0;
			virtual std::uint32_t GetSampleCount()const = 0;
			virtual int GetSampleQuality()const = 0;
			virtual RenderFormat GetRenderFormat()const = 0;
			virtual std::uint32_t GetCurrentBackBufferIndex()const = 0;
			//get corresponding render target
			virtual IRenderTarget* GetDefaultRenderTarget() = 0;
		};
	}
}
