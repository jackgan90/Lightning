#pragma once
#include <memory>
#include "IRefObject.h"
#include "IRenderer.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::IRenderer;
		using Plugins::IRefObject;
		class IDrawable : public IRefObject
		{
		public:
			virtual void Draw(IRenderer* , const SceneRenderData&) = 0;
		};
	}
}
