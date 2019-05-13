#pragma once
#include "IRenderer.h"
#include "ICamera.h"
#include "IDrawable.h"
#include "IRenderTarget.h"
#include "IDepthStencilBuffer.h"

namespace Lightning
{
	namespace Render
	{
		struct IRenderPass
		{
			virtual ~IRenderPass() = default;
			//Do the actual render,different algorithms will be carried out in this step
			virtual void Render(IRenderer& renderer) = 0;
			//Adds a drawable to the pass,if the pass accepts this drawable, true is returned,otherwise false is returned.
			virtual bool AddDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera) = 0;
			//Tells the pass that Renderer will begin to render,the pass should not accept Drawables in this frame anymore
			virtual void BeginRender(IRenderer& renderer) = 0;
			//EndRender means the renderer finishes work and the cached drawables are safe to discard
			virtual void EndRender(IRenderer& renderer) = 0;
			//Gets the render target count of this pass
			virtual std::size_t GetRenderTargetCount()const = 0;
			//Gets the render target by index
			virtual std::shared_ptr<IRenderTarget> GetRenderTarget(std::size_t index)const = 0;
			//Gets the depth stencil buffer of this pass
			virtual std::shared_ptr<IDepthStencilBuffer> GetDepthStencilBuffer()const = 0;
		};
	}
}