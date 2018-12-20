#pragma once
#include <memory>
#include "RefCount.h"
#include "IRenderer.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::IRenderer;
		using Plugins::RefCount;
		class IDrawable : public RefCount
		{
		public:
			virtual void Draw(IRenderer* , const SceneRenderData&) = 0;
		};
	}
}
