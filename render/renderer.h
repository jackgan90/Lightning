#pragma once
#include <memory>
#include <vector>
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
			std::uint64_t GetCurrentFrameCount()const override;
			void SetClearColor(const ColorF& color)override;
			void Draw(const RenderItem& item)override;
			//TODO there can be multiple render passes in effect simultaneously,shoulc change it
			void AddRenderPass(RenderPassType type)override;
			std::size_t GetFrameResourceIndex()const override;
			static Renderer* Instance() { return s_instance; }
		protected:
			struct FrameResource
			{

			};
			Renderer(const SharedFileSystemPtr& fs, RenderPassType renderPassType = RenderPassType::FORWARD);
			virtual void BeginFrame();
			virtual void DoFrame();
			virtual void EndFrame();
			virtual void ApplyRenderPass();
			static Renderer* s_instance;
			std::uint64_t m_frameCount;
			SharedFileSystemPtr m_fs;
			std::unique_ptr<IDevice> m_device;
			std::unique_ptr<ISwapChain> m_swapChain;
			std::vector<std::unique_ptr<RenderPass>> m_renderPasses;
			std::size_t m_currentBackBufferIndex;
			ColorF m_clearColor;
			SharedDepthStencilBufferPtr m_depthStencilBuffer;
		};
	}
}