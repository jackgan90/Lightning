#pragma once
#include "IDrawable.h"
#include "IRenderer.h"
#include "ICamera.h"

namespace Lightning
{
	namespace World
	{
		struct IRenderable : Render::IDrawable
		{
			virtual void Render(Render::IRenderer& renderer, const std::shared_ptr<Render::ICamera>& camera) = 0;
		};
	}
}