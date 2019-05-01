#pragma once
#include "IRefObject.h"
#include "IRenderer.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::IRenderer;
		using Plugins::IRefObject;
		struct IDrawable
		{
			virtual ~IDrawable() = default;
			virtual void Draw(IRenderer* , const SceneRenderData&) = 0;
		};
	}
}
