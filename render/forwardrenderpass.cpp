#include "forwardrenderpass.h"

namespace LightningGE
{
	namespace Render
	{
		void ForwardRenderPass::Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material)
		{
			m_renderItems.emplace_back(geometry, material);
		}

		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void ForwardRenderPass::Apply()
		{
			for (auto& renderItem : m_renderItems)
			{

			}
			m_renderItems.clear();
		}
	}
}