#pragma once
#include <memory>
#include "irenderer.h"
#include "filesystem.h"
#include "renderpass.h"

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
			void Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material)override;
			static Renderer* Instance() { return s_instance; }
		protected:
			virtual void BeginFrame() = 0;
			virtual void DoFrame() = 0;
			virtual void EndFrame() = 0;
			unsigned int m_frameIndex;
			SharedFileSystemPtr m_fs;
			std::unique_ptr<IDevice> m_device;
		private:
			static Renderer* s_instance;
		};
	}
}