#include "irendertargetmanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		SharedRenderTargetPtr RenderTargetManager::GetRenderTarget(const RenderTargetID& targetID)
		{
			auto it = m_renderTargets.find(targetID);
			if (it == m_renderTargets.end())
			{
				return SharedRenderTargetPtr();
			}
			return it->second;
		}
	}
}