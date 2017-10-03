#include "renderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		void Renderer::Render()
		{
			BeginRender();
			DoRender();
			EndRender();
		}
	}
}