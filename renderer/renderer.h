#pragma once
#include "irenderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API Renderer : public IRenderer
		{
		public:
			void Render()override;
		protected:
			virtual void BeginRender() = 0;
			virtual void DoRender() = 0;
			virtual void EndRender() = 0;
		};
	}
}