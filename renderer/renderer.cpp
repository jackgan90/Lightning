#include "renderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		Renderer::Renderer() :m_frameIndex(0)
		{

		}

		void Renderer::Render()
		{
			BeginRender();
			DoRender();
			EndRender();
		}

		unsigned long Renderer::GetCurrentFrameIndex()const
		{
			return m_frameIndex;
		}
	}
}