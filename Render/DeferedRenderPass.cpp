#include "DeferedRenderPass.h"

namespace Lightning
{
	namespace Render
	{
		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void DeferedRenderPass::Apply()
		{

		}

		void DeferedRenderPass::OnAddRenderNode(const RenderNode& node)
		{

		}

		void DeferedRenderPass::OnFrameBegin()
		{

		}

		void DeferedRenderPass::OnFrameUpdate()
		{

		}
	}
}