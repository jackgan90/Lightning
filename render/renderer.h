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
			~Renderer()override;
			//entry point of render system
			void Render()override;
			IDevice* GetDevice()override;
			ISwapChain* GetSwapChain()override;
			//return the current frame index
			std::uint64_t GetCurrentFrameIndex()const override;
			void Draw(const RenderItem& item)override;
			void SetRenderPass(RenderPassType type)override;
			static Renderer* Instance() { return s_instance; }
		protected:
			Renderer(const SharedFileSystemPtr& fs, RenderPassType renderPassType = RenderPassType::FORWARD);
			virtual void BeginFrame() = 0;
			virtual void DoFrame() = 0;
			virtual void EndFrame() = 0;
			virtual void ApplyRenderPass();
			std::uint64_t m_frameIndex;
			SharedFileSystemPtr m_fs;
			std::unique_ptr<IDevice> m_device;
			std::unique_ptr<ISwapChain> m_swapChain;
			std::unique_ptr<RenderPass> m_renderPass;
			static Renderer* s_instance;
		};
	}
}