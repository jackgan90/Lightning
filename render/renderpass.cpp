#include "renderpass.h"
#include "renderer.h"

namespace Lightning
{
	namespace Render
	{
		RenderPass::RenderPass(RenderPassType type):mType(type)
		{
			Renderer::Instance()->RegisterCallback(RendererEvent::FRAME_BEGIN, [this]() {OnFrameBegin(); });
			Renderer::Instance()->RegisterCallback(RendererEvent::FRAME_POST_UPDATE, [this]() {OnFrameUpdate(); });
		}
	}
}