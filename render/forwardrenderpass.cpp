#include "forwardrenderpass.h"

namespace LightningGE
{
	namespace Render
	{
		void ForwardRenderPass::Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material)
		{

		}

		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void ForwardRenderPass::Apply()
		{

		}
	}
}