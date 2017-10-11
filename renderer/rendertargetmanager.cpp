#include "irendertargetmanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		RenderTargetPtr RenderTargetManager::GetRenderTarget(const RenderTargetID& targetID)
		{
			auto it = m_renderTargets.find(targetID);
			if (it == m_renderTargets.end())
			{
				return RenderTargetPtr();
			}
			return it->second;
		}
	}
}