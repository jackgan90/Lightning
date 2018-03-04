#include "deferedrenderpass.h"

namespace LightningGE
{
	namespace Render
	{
		void DeferedRenderPass::Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material, const Transform& transform)
		{

		}

		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void DeferedRenderPass::Apply()
		{

		}
	}
}