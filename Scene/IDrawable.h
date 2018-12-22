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
		struct IDrawable : IRefObject
		{
			virtual void INTERFACECALL Draw(IRenderer* , const SceneRenderData&) = 0;
		};
	}
}
