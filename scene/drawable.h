#pragma once
#include <memory>
#include "sceneexportdef.h"
#include "renderer.h"
#include "scenerenderdata.h"

namespace LightningGE
{
	namespace Scene
	{
		using Render::Renderer;
		class LIGHTNINGGE_SCENE_API IDrawable
		{
		public:
			virtual ~IDrawable() = default;
			virtual void Draw(Renderer& renderer, const SceneRenderData& sceneRenderData) = 0;
		};
		using SharedDrawablePtr = std::shared_ptr<IDrawable>;
	}
}