#pragma once
#include <memory>
#include "SceneExportDef.h"
#include "Renderer.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::IRenderer;
		class IDrawable
		{
		public:
			virtual ~IDrawable() = default;
			virtual void Draw(IRenderer&, const SceneRenderData&) = 0;
		};
		using SharedDrawablePtr = std::shared_ptr<IDrawable>;
	}
}
