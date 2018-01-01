#pragma once
#include <memory>
#include "irenderer.h"
#include "filesystem.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::SharedFileSystemPtr;
		class LIGHTNINGGE_RENDER_API Renderer : public IRenderer
		{
		public:
			Renderer(const SharedFileSystemPtr& fs);
			~Renderer()override;
			//entry point of render system
			void Render()override;
			IDevice* GetDevice()override;
			//return the current frame index
			unsigned long GetCurrentFrameIndex()const override;
			static Renderer* Instance() { return s_instance; }
		protected:
			virtual void BeginRender() = 0;
			virtual void DoRender() = 0;
			virtual void EndRender() = 0;
			unsigned int m_frameIndex;
			SharedFileSystemPtr m_fs;
			std::unique_ptr<IDevice> m_device;
		private:
			static Renderer* s_instance;
		};
	}
}