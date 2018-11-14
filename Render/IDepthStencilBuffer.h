#pragma once
#include <cstdint>
#include <memory>
#include "RendererExportDef.h"
#include "RenderConstants.h"

namespace Lightning
{
	namespace Render
	{
		class IDepthStencilBuffer
		{
		public:
			virtual ~IDepthStencilBuffer() = default;
			virtual void SetClearValue(float depthValue=1.0f, std::uint32_t stencilValue=0) = 0;
			virtual float GetDepthClearValue()const = 0;
			virtual std::uint32_t GetStencilClearValue()const = 0;
			virtual RenderFormat GetRenderFormat()const = 0;
			virtual std::uint32_t GetWidth()const = 0;
			virtual std::uint32_t GetHeight()const = 0;
		};
		using SharedDepthStencilBufferPtr = std::shared_ptr<IDepthStencilBuffer>;
	}
}
