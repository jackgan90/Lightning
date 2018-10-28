#pragma once
#include <memory>
#include "container.h"
#include "irenderer.h"
#include "irenderfence.h"
#include "filesystem.h"
#include "renderpass.h"
#include "iwindow.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::SharedFileSystemPtr;
		using Foundation::container;
		using WindowSystem::SharedWindowPtr;

		struct FrameResource
		{
			IRenderFence *fence{ nullptr };
			std::uint64_t frame{ 0 };
			SharedDepthStencilBufferPtr defaultDepthStencilBuffer;
			RenderQueue renderQueue;

			void BeginFrame()
			{
				renderQueue.clear();
			}

			void Release()
			{
				if (fence)
				{
					delete fence;
					fence = nullptr;
				}
				defaultDepthStencilBuffer.reset();
				renderQueue.clear();
			}
		};

		class LIGHTNING_RENDER_API Renderer : public IRenderer
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
			void AddRenderNode(const RenderNode& item)override;
			//TODO there can be multiple render passes in effect simultaneously,shoulc change it
			void AddRenderPass(RenderPassType type)override;
			std::size_t GetFrameResourceIndex()const override;
			void Start()override;
			void ShutDown()override;
			void RegisterCallback(IRendererCallback* callback)override;
			static IRenderer* Instance() { return sInstance; }
			IWindow* GetOutputWindow()override { return mOutputWindow.get(); }
			const RenderQueue& GetRenderQueue()override;
			SharedDepthStencilBufferPtr GetDefaultDepthStencilBuffer()override;
		protected:
			Renderer(const SharedFileSystemPtr& fs, const SharedWindowPtr& pWindow);
			void WaitForPreviousFrame(bool waitAll);
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
			virtual RenderPass* CreateRenderPass(RenderPassType type);
			static IRenderer* sInstance;
			std::uint64_t mFrameCount;
			SharedFileSystemPtr mFs;
			std::unique_ptr<IDevice> mDevice;
			std::unique_ptr<ISwapChain> mSwapChain;
			container::vector<std::unique_ptr<RenderPass>> mRenderPasses;
			std::size_t mFrameResourceIndex;
			ColorF mClearColor;
			container::vector<IRendererCallback*> mCallbacks;
			FrameResource mFrameResources[RENDER_FRAME_COUNT];
			SharedWindowPtr mOutputWindow;
		};
	}
}
