#pragma once
#include "IRenderer.h"
#include "ICamera.h"

namespace Lightning
{
	namespace Render
	{
		using Render::IRenderer;
		struct IDrawable
		{
			virtual ~IDrawable() = default;
			virtual void Draw(IRenderer* renderer, const std::shared_ptr<ICamera>& camera) = 0;
		};
	}
}
