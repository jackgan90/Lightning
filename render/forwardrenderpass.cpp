#include "forwardrenderpass.h"
#include "renderer.h"

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
				CommitBuffers(renderItem.geometry);
			}
			m_renderItems.clear();
		}

		void ForwardRenderPass::CommitBuffers(const SharedGeometryPtr& geometry)
		{
			auto pDevice = Renderer::Instance()->GetDevice();
			for (size_t i = 0; i < MAX_GEOMETRY_BUFFER_COUNT; i++)
			{
				if (!geometry->vbs[i])
					continue;
				if (geometry->vbs_dirty[i])
				{
					pDevice->CommitGPUBuffer(geometry->vbs[i].get());
					geometry->vbs_dirty[i] = false;
				}
			}
			if (geometry->ib && geometry->ib_dirty)
			{
				pDevice->CommitGPUBuffer(geometry->ib.get());
				geometry->ib_dirty = false;
			}
		}
	}
}