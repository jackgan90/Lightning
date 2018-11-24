#include "RenderPass.h"
#include "Renderer.h"

namespace Lightning
{
	namespace Render
	{
		RenderPass::RenderPass(RenderPassType type):mType(type)
		{
			Renderer::Instance()->RegisterCallback(RendererEvent::FRAME_END, [this]() {OnFrameEnd(); });
		}
	}
}