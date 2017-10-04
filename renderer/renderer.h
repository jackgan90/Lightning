#pragma once
#include "irenderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API Renderer : public IRenderer
		{
		public:
			Renderer();
			void Render()override;
			unsigned long GetCurrentFrameIndex()const override;
		protected:
			virtual void BeginRender() = 0;
			virtual void DoRender() = 0;
			virtual void EndRender() = 0;
			unsigned int m_frameIndex;
		};
	}
}