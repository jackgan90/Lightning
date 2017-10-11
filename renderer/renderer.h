#pragma once
#include "irenderer.h"
#include "filesystem.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::FileSystemPtr;
		class LIGHTNINGGE_RENDERER_API Renderer : public IRenderer
		{
		public:
			Renderer(const FileSystemPtr& fs);
			//entry point of render system
			void Render()override;
			//return the current frame index
			unsigned long GetCurrentFrameIndex()const override;
		protected:
			virtual void BeginRender() = 0;
			virtual void DoRender() = 0;
			virtual void EndRender() = 0;
			unsigned int m_frameIndex;
			FileSystemPtr m_fs;
		};
	}
}