#include "DeferedRenderPass.h"

namespace Lightning
{
	namespace Render
	{
		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void DeferedRenderPass::Apply(RenderQueue& renderQueue)
		{

		}

		void DeferedRenderPass::OnAddRenderUnit(const IRenderUnit* unit)
		{

		}

		void DeferedRenderPass::OnFrameEnd()
		{

		}
	}
}