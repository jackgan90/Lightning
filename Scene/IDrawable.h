#pragma once
#include <memory>
#include "RefObject.h"
#include "IRenderer.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::IRenderer;
		using Plugins::RefObject;
		class IDrawable : public RefObject
		{
		public:
			virtual void Draw(IRenderer* , const SceneRenderData&) = 0;
		};
	}
}
