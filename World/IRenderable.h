#pragma once
#include "ICamera.h"
#include "IRenderer.h"

namespace Lightning
{
	namespace World
	{
		struct IRenderable
		{
			virtual void Render(Render::IRenderer& renderer, const std::shared_ptr<Render::ICamera>& camera) = 0;
		};
	}
}