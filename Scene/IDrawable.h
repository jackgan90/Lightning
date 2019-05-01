#pragma once
#include "IRenderer.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::IRenderer;
		struct IDrawable
		{
			virtual ~IDrawable() = default;
			virtual void Draw(IRenderer* , const SceneRenderData&) = 0;
		};
	}
}
