#pragma once
#include <memory>
#include "Container.h"
#include "IRenderer.h"
#include "IRenderFence.h"
#include "IFileSystem.h"
#include "RenderPass.h"
#include "IWindow.h"

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
			void INTERFACECALL CommitRenderNode(const RenderNode& item)override;
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
			struct SemanticInfo
			{
				std::string rawName;
				std::string name;
				SemanticIndex index;
			};
			Renderer(Window::IWindow* window);
			void WaitForPreviousFrame(bool waitAll);
			virtual void OnFrameBegin() = 0;
			virtual void OnFrameUpdate() = 0;
			virtual void OnFrameEnd() = 0;
			//CreateRenderFence is called in Start after the creation of device and swap chain
			virtual IRenderFence* CreateRenderFence() = 0;
			//CreateDevice is called first in Start
			virtual IDevice* CreateDevice() = 0;
			//CreateSwapChain is called in Start,ensuring the device is already created
			virtual ISwapChain* CreateSwapChain() = 0;
			virtual IDepthStencilBuffer* CreateDepthStencilBuffer(std::uint32_t width, std::uint32_t height) = 0;
			virtual RenderPass* CreateRenderPass(RenderPassType type);
			void ApplyRenderPasses();
			void GetSemanticInfo(RenderSemantics semantic, SemanticIndex& index, std::string& name);
			static IRenderer* sInstance;
			std::uint64_t mFrameCount;
			std::unique_ptr<IDevice> mDevice;
			std::unique_ptr<ISwapChain> mSwapChain;
			Container::Vector<std::unique_ptr<RenderPass>> mRenderPasses;
			std::size_t mFrameResourceIndex;
			ColorF mClearColor;
			FrameResource mFrameResources[RENDER_FRAME_COUNT];
			std::size_t mRenderQueueIndex;
			RenderQueue mRenderQueues[RENDER_FRAME_COUNT + 1];
			RenderQueue* mCurrentFrameRenderQueue;
			Window::IWindow* mOutputWindow;
			bool mStarted;
			Container::UnorderedMap<RenderSemantics, SemanticInfo> mPipelineInputSemanticInfos;
			Container::UnorderedMap<std::string, RenderSemantics> mUniformToSemantics;
			Container::UnorderedMap<RenderSemantics, const char*> mSemanticsToUniform;
		private:
			SemanticInfo ParsePipelineInputSemantics(const SemanticItem& item);
		};
	}
}
