#pragma once
#include <memory>
#include "SceneExportDef.h"
#include "renderer.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::IRenderer;
		class LIGHTNING_SCENE_API IDrawable
		{
		public:
			virtual ~IDrawable() = default;
			virtual void Draw(IRenderer&, const SceneRenderData&) = 0;
		};
		using SharedDrawablePtr = std::shared_ptr<IDrawable>;
	}
}
