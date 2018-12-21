#pragma once
#include "IPlugin.h"
#include "IMaterial.h"
#include "IRenderer.h"
#include "IWindow.h"

namespace Lightning
{
	namespace Plugins
	{
		//Users of RenderPlugin should call CreateRenderer first and then GetRenderer will give
		//the renderer instance created by CreateRenderer.Finally DestroyRenderer should be called
		//note that CreateRenderer and DestroyRenderer are not thread safe.So ensure there's no concurrent
		//call for the two methods.
		struct IRenderPlugin : public IPlugin
		{
			virtual Render::IMaterial* INTERFACECALL CreateMaterial() = 0;
			virtual Render::IRenderer* INTERFACECALL GetRenderer() = 0;
			//Thread unsafe
			virtual Render::IRenderer* INTERFACECALL CreateRenderer(Window::IWindow*) = 0;
			//Thread unsafe
			virtual void INTERFACECALL DestroyRenderer(Render::IRenderer*) = 0;
		};
	}
}