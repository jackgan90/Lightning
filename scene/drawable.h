#pragma once
#include <memory>
#include "sceneexportdef.h"
#include "renderer.h"
#include "scenerenderdata.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::Renderer;
		class LIGHTNING_SCENE_API IDrawable
		{
		public:
			virtual ~IDrawable() = default;
			virtual void Draw(Renderer& renderer, const SceneRenderData& sceneRenderData) = 0;
		};
		using SharedDrawablePtr = std::shared_ptr<IDrawable>;
	}
}
