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

		struct FrameFenceValue
		{
			std::uint64_t frame;
			std::uint64_t fenceValue;

			bool operator>(const FrameFenceValue& other)const noexcept
			{
				return fenceValue > other.fenceValue;
			}
		};

		struct FrameResource
		{
			IRenderFence *fence{ nullptr };
			std::priority_queue<FrameFenceValue, container::vector<FrameFenceValue>, std::greater<FrameFenceValue>> frameEndMarkers;
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
			void Draw(const RenderItem& item)override;
			//TODO there can be multiple render passes in effect simultaneously,shoulc change it
			void AddRenderPass(RenderPassType type)override;
			std::size_t GetFrameResourceIndex()const override;
			void Start()override;
			void ShutDown()override;
			void RegisterCallback(IRendererCallback* callback)override;
			static Renderer* Instance() { return sInstance; }
			IWindow* GetOutputWindow()override { return mOutputWindow.get(); }
		protected:
			Renderer(const SharedFileSystemPtr& fs, const SharedWindowPtr& pWindow, RenderPassType renderPassType = RenderPassType::FORWARD);
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
			static Renderer* sInstance;
			std::uint64_t mFrameCount;
			SharedFileSystemPtr mFs;
			std::unique_ptr<IDevice> mDevice;
			std::unique_ptr<ISwapChain> mSwapChain;
			container::vector<std::unique_ptr<RenderPass>> mRenderPasses;
			std::size_t mCurrentBackBufferIndex;
			ColorF mClearColor;
			SharedDepthStencilBufferPtr mDepthStencilBuffer;
			container::vector<IRendererCallback*> mCallbacks;
			FrameResource mFrameResources[RENDER_FRAME_COUNT];
			SharedWindowPtr mOutputWindow;
		};
	}
}
