#include <algorithm>
#include "shadermanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		void ShaderManager::ReleaseRenderResources()
		{
			std::for_each(m_shaders.begin(), m_shaders.end(),
				[](std::pair<const std::size_t, ShaderPtr> pair) {pair.second->ReleaseRenderResources(); });
			m_shaders.clear();
		}

	}
}