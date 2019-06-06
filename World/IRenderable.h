#pragma once
#include "IDrawable.h"
#include "IRenderer.h"
#include "ICamera.h"

namespace Lightning
{
	namespace World
	{
		//A type of object that is capable of being drawn by renderer.
		struct IRenderable : Render::IDrawable
		{
			//NeedRender is called before sending render request to renderer.It makes implementors have the
			//chance to disable render in some conditions.
			virtual bool NeedRender()const = 0;
			//Send render request to the renderer.A draw called is expected to be issued by the underlying renderer after this call.
			virtual void Render(Render::IRenderer& renderer, const std::shared_ptr<Render::ICamera>& camera) = 0;
		};
	}
}