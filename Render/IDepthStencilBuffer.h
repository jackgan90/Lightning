#pragma once
#include <cstdint>
#include <memory>
#include "Texture/ITexture.h"

namespace Lightning
{
	namespace Render
	{
		struct IDepthStencilBuffer
		{
			virtual ~IDepthStencilBuffer() = default;
			virtual void SetClearValue(float depthValue=1.0f, std::uint32_t stencilValue=0) = 0;
			virtual float GetDepthClearValue()const = 0;
			virtual std::uint8_t GetStencilClearValue()const = 0;
			virtual std::shared_ptr<ITexture> GetTexture()const = 0;
		};
	}
}
