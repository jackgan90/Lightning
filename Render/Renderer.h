#pragma once
#include "IRenderer.h"
#include "IRenderFence.h"
#include "SwapChain.h"
#include "Device.h"
#include "RenderPass/IRenderPass.h"

namespace Lightning
{
	namespace Render
	{
		struct FrameResource
		{
			std::unique_ptr<IRenderFence> fence;
			std::uint64_t frame{ 0 };
			std::shared_ptr<IDepthStencilBuffer> defaultDepthStencilBuffer;

			void Release();
		};

		class Renderer : public IRenderer
		{
		public:
			~Renderer()override;
			//entry point of render system
			void Render()override;
			IDevice* GetDevice()override;
			ISwapChain* GetSwapChain()override;
			//return the current frame index
			std::uint64_t GetCurrentFrameCount()const override;
			std::size_t GetFrameResourceIndex()const override;
			void Start()override;
			void ShutDown()override;
			static IRenderer* Instance() { return sInstance; }
			Window::IWindow* GetOutputWindow()override { return mOutputWindow; }
			std::shared_ptr<IDepthStencilBuffer> GetDefaultDepthStencilBuffer()override;
			std::shared_ptr<IRenderTarget> GetDefaultRenderTarget()override;
			RenderSemantics GetUniformSemantic(const char* uniform_name)override;
			const char* GetUniformName(RenderSemantics semantic)override;
			void GetSemanticInfo(RenderSemantics semantic, SemanticIndex& index, std::string& name)override;
			void Draw(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera)override;
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
			virtual bool CheckIfDepthStencilBufferNeedsResize();
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
			std::unique_ptr<IRenderPass> mRootRenderPass;
			FrameResource mFrameResources[RENDER_FRAME_COUNT];
			Window::IWindow* mOutputWindow;
			std::unordered_map<RenderSemantics, SemanticInfo> mPipelineInputSemanticInfos;
			std::unordered_map<std::string, RenderSemantics> mUniformToSemantics;
			std::unordered_map<RenderSemantics, const char*> mSemanticsToUniform;
		private:
			void HandleWindowResize();
			SemanticInfo ParsePipelineInputSemantics(const SemanticItem& item);
			std::size_t mFrameResourceIndex;
			bool mStarted;
			std::uint64_t mFrameCount;
		};
	}
}
