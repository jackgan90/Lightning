#pragma once
#include <memory>
#include <vector>
#include "irenderer.h"
#include "irenderfence.h"
#include "filesystem.h"
#include "renderpass.h"
#include "iwindow.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::SharedFileSystemPtr;
		using WindowSystem::SharedWindowPtr;

		struct FrameResource
		{
			IRenderFence *fence;
			void Release(bool perFrame)
			{
				if (!perFrame)
				{
					if (fence)
					{
						delete fence;
						fence = nullptr;
					}
				}
			}
		};

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
			void Start()override;
			void ShutDown()override;
			static Renderer* Instance() { return s_instance; }
			IWindow* GetOutputWindow()override { return m_outputWindow.get(); }
		protected:
			Renderer(const SharedFileSystemPtr& fs, const SharedWindowPtr& pWindow, RenderPassType renderPassType = RenderPassType::FORWARD);
			void WaitForPreviousFrame(bool waitAll);
			void ReleasePreviousFrameResources(bool perFrame);
			virtual void BeginFrame();
			virtual void DoFrame();
			virtual void EndFrame();
			virtual void ApplyRenderPass();
			//CreateRenderFence is called in Start after the creation of device and swap chain
			virtual IRenderFence* CreateRenderFence() = 0;
			//CreateDevice is called first in Start
			virtual IDevice* CreateDevice() = 0;
			//CreateSwapChain is called in Start,ensuring the device is already created
			virtual ISwapChain* CreateSwapChain() = 0;
			virtual IDepthStencilBuffer* CreateDepthStencilBuffer(std::size_t width, std::size_t height) = 0;
			static Renderer* s_instance;
			std::uint64_t m_frameCount;
			SharedFileSystemPtr m_fs;
			std::unique_ptr<IDevice> m_device;
			std::unique_ptr<ISwapChain> m_swapChain;
			std::vector<std::unique_ptr<RenderPass>> m_renderPasses;
			std::size_t m_currentBackBufferIndex;
			ColorF m_clearColor;
			SharedDepthStencilBufferPtr m_depthStencilBuffer;
			FrameResource m_frameResources[RENDER_FRAME_COUNT];
			SharedWindowPtr m_outputWindow;
		};
	}
}