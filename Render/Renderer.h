#pragma once
#include "Container.h"
#include "IRenderer.h"
#include "IRenderFence.h"
#include "IFileSystem.h"
#include "IWindow.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "Device.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;

		struct FrameResource
		{
			IRenderFence *fence{ nullptr };
			std::uint64_t frame{ 0 };
			IDepthStencilBuffer* defaultDepthStencilBuffer{ nullptr };
			RenderQueue* renderQueue{ nullptr };

			void ReleaseRenderQueue();
			void OnFrameBegin();
			void Release();
		};

		class Renderer : public IRenderer
		{
		public:
			~Renderer()override;
			//entry point of render system
			void INTERFACECALL Render()override;
			IDevice* INTERFACECALL GetDevice()override;
			ISwapChain* INTERFACECALL GetSwapChain()override;
			//return the current frame index
			std::uint64_t INTERFACECALL GetCurrentFrameCount()const override;
			void INTERFACECALL SetClearColor(float r, float g, float b, float a)override;
			IRenderUnit* INTERFACECALL CreateRenderUnit()override;
			void INTERFACECALL CommitRenderUnit(const IRenderUnit* item)override;
			//TODO there can be multiple render passes in effect simultaneously,should change it
			void INTERFACECALL AddRenderPass(RenderPassType type)override;
			std::size_t INTERFACECALL GetFrameResourceIndex()const override;
			void INTERFACECALL Start()override;
			void INTERFACECALL ShutDown()override;
			static IRenderer* Instance() { return sInstance; }
			Window::IWindow* INTERFACECALL GetOutputWindow()override { return mOutputWindow; }
			IDepthStencilBuffer* INTERFACECALL GetDefaultDepthStencilBuffer()override;
			RenderSemantics INTERFACECALL GetUniformSemantic(const char* uniform_name)override;
			const char* INTERFACECALL GetUniformName(RenderSemantics semantic)override;
		protected:
			Renderer(Window::IWindow* window);
			//Thread unsafe ,must ensure there's no concurrent execution
			void WaitForPreviousFrame(bool waitAll);
			virtual void OnFrameBegin() = 0;
			virtual void OnFrameUpdate() = 0;
			//Thread unsafe, cannot run parallel
			virtual void OnFrameEnd() = 0;
			virtual void ResizeDepthStencilBuffer(IDepthStencilBuffer* depthStencilBuffer, std::size_t width, std::size_t height) = 0;
			//CreateRenderFence is called in Start after the creation of device and swap chain
			virtual IRenderFence* CreateRenderFence() = 0;
			//CreateDevice is called first in Start
			virtual Device* CreateDevice() = 0;
			//CreateSwapChain is called in Start,ensuring the device is already created
			virtual SwapChain* CreateSwapChain() = 0;
			virtual RenderPass* CreateRenderPass(RenderPassType type);
			virtual bool CheckIfDepthStencilBufferNeedsResize();
			void ApplyRenderPasses();
			void GetSemanticInfo(RenderSemantics semantic, SemanticIndex& index, std::string& name);
			void ResetFrameRenderQueue();
		protected:
			struct SemanticInfo
			{
				std::string rawName;
				std::string name;
				SemanticIndex index;
			};
			//Member fields
			static IRenderer* sInstance;
			std::unique_ptr<Device> mDevice;
			std::unique_ptr<SwapChain> mSwapChain;
			Container::Vector<std::unique_ptr<RenderPass>> mRenderPasses;
			ColorF mClearColor;
			FrameResource mFrameResources[RENDER_FRAME_COUNT];
			Window::IWindow* mOutputWindow;
			Container::UnorderedMap<RenderSemantics, SemanticInfo> mPipelineInputSemanticInfos;
			Container::UnorderedMap<std::string, RenderSemantics> mUniformToSemantics;
			Container::UnorderedMap<RenderSemantics, const char*> mSemanticsToUniform;
		private:
			void HandleWindowResize();
			SemanticInfo ParsePipelineInputSemantics(const SemanticItem& item);
			std::size_t mFrameResourceIndex;
			bool mStarted;
			std::uint64_t mFrameCount;
			std::size_t mRenderQueueIndex;
			RenderQueue mRenderQueues[RENDER_FRAME_COUNT + 1];
			RenderQueue* mCurrentFrameRenderQueue;
		};
	}
}
