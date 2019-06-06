#pragma once
#include "Texture/ITexture.h"

namespace Lightning
{
	namespace Render
	{
		using RenderTargetID = int;
		struct IRenderTarget
		{
			//return the attached RT ID
			virtual ~IRenderTarget() = default;
			virtual RenderTargetID GetID()const = 0;
			virtual std::shared_ptr<ITexture> GetTexture()const = 0;
		};
	}
}
