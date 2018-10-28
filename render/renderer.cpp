#include <cassert>
#include "device.h"
#include "renderer.h"
#include "forwardrenderpass.h"
#include "deferedrenderpass.h"
#include "framememoryallocator.h"

#define INVOKE_CALLBACK(Callback)\
for(auto& callback : mCallbacks)\
{\
	callback->Callback();\
}

namespace Lightning
{
	namespace Render
	{
		IRenderer* Renderer::sInstance{ nullptr };
		Foundation::FrameMemoryAllocator g_RenderAllocator;
		Renderer::Renderer(const SharedFileSystemPtr& fs, const SharedWindowPtr& pWindow) :
			mOutputWindow(pWindow),
			mFrameCount(0), mFs(fs), mFrameResourceIndex(0), mClearColor(0.5f, 0.5f, 0.5f, 1.0f)
		{
			assert(!sInstance);
			sInstance = this;
		}

		Renderer::~Renderer()
		{
			assert(sInstance == this);
			sInstance = nullptr;
			mDevice.reset();
			mSwapChain.reset();
		}

		void Renderer::Render()
		{
			BeginFrame();
			DoFrame();
			ApplyRenderPass();
			EndFrame();
		}

		void Renderer::BeginFrame()
		{
			WaitForPreviousFrame(false);
			mFrameCount++;
			mFrameResourceIndex = mSwapChain->GetCurrentBackBufferIndex();
			mFrameResources[mFrameResourceIndex].BeginFrame();
			mDevice->BeginFrame(mFrameResourceIndex);
			for (auto& pass : mRenderPasses)
			{
				pass->OnBeginFrame();
			}
			INVOKE_CALLBACK(OnBeginFrame)
		}

		void Renderer::DoFrame()
		{
			auto defaultRenderTarget = mSwapChain->GetDefaultRenderTarget();
			ClearRenderTarget(defaultRenderTarget, mClearColor);
			auto depthStencilBuffer = GetDefaultDepthStencilBuffer();
			ClearDepthStencilBuffer(depthStencilBuffer, DepthStencilClearFlags::CLEAR_DEPTH | DepthStencilClearFlags::CLEAR_STENCIL,
				depthStencilBuffer->GetDepthClearValue(), depthStencilBuffer->GetStencilClearValue(), nullptr);
			INVOKE_CALLBACK(OnDoFrame)
		}

		void Renderer::EndFrame()
		{
			INVOKE_CALLBACK(OnEndFrame)
			auto fence = mFrameResources[mFrameResourceIndex].fence;
			mFrameResources[mFrameResourceIndex].frame = mFrameCount;
			fence->SetTargetValue(mFrameCount);
			mDevice->EndFrame(mFrameResourceIndex);
			mSwapChain->Present();
			g_RenderAllocator.FinishFrame(mFrameCount);
		}


		void Renderer::SetClearColor(const ColorF& color)
		{
			mClearColor = color;
		}

		void Renderer::ApplyRenderPass()
		{
			for (auto& pass : mRenderPasses)
			{
				pass->Apply();
			}
		}

		std::uint64_t Renderer::GetCurrentFrameCount()const
		{
			return mFrameCount;
		}

		IDevice* Renderer::GetDevice()
		{
			return mDevice.get();
		}

		ISwapChain* Renderer::GetSwapChain()
		{
			return mSwapChain.get();
		}


		void Renderer::AddRenderNode(const RenderNode& node)
		{
			mFrameResources[mFrameResourceIndex].renderQueue.push_back(node);
			for (auto& pass : mRenderPasses)
			{
				pass->OnAddRenderNode(node);
			}
		}

		void Renderer::AddRenderPass(RenderPassType type)
		{
			auto pass = CreateRenderPass(type);
			if(pass)
				mRenderPasses.emplace_back(pass);
		}

		std::size_t Renderer::GetFrameResourceIndex()const
		{
			return mFrameResourceIndex;
		}

		void Renderer::RegisterCallback(IRendererCallback* callback)
		{
			mCallbacks.push_back(callback);
		}

		RenderPass* Renderer::CreateRenderPass(RenderPassType type)
		{
			switch (type)
			{
			case RenderPassType::FORWARD:
				return new ForwardRenderPass();
			case RenderPassType::DEFERED:
				return new DeferedRenderPass();
			default:
				break;
			}
			return nullptr;
		}

		void Renderer::Start()
		{
			mDevice.reset(CreateDevice());
			mSwapChain.reset(CreateSwapChain());
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				mFrameResources[i].fence = CreateRenderFence();
				mFrameResources[i].defaultDepthStencilBuffer.reset(
					CreateDepthStencilBuffer( mOutputWindow->GetWidth(), mOutputWindow->GetHeight()));
			}
			mFrameResourceIndex = mSwapChain->GetCurrentBackBufferIndex();
			AddRenderPass(RenderPassType::FORWARD);
		}

		void Renderer::ShutDown()
		{
			WaitForPreviousFrame(true);
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mFrameResources[i].Release();
			}
			mOutputWindow.reset();
			mDevice.reset();
			mSwapChain.reset();
			mRenderPasses.clear();
		}

		const RenderQueue& Renderer::GetRenderQueue()
		{
			return mFrameResources[mFrameResourceIndex].renderQueue;
		}

		SharedDepthStencilBufferPtr Renderer::GetDefaultDepthStencilBuffer()
		{
			return mFrameResources[mFrameResourceIndex].defaultDepthStencilBuffer;
		}

		void Renderer::WaitForPreviousFrame(bool waitAll)
		{
			container::vector<UINT> bufferIndice;
			if (!waitAll)
			{
				bufferIndice.push_back(mSwapChain->GetCurrentBackBufferIndex());
			}
			else
			{
				for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					bufferIndice.push_back(i);
					//explicit signal to prevent release assert
					mFrameResources[i].fence->SetTargetValue(mFrameResources[i].fence->GetTargetValue() + 1);
				}
			}
			for (const auto& bufferIndex : bufferIndice)
			{
				auto& frameResource = mFrameResources[bufferIndex];
				frameResource.fence->WaitForTarget();
				g_RenderAllocator.ReleaseFramesBefore(frameResource.frame);
			}
		}


	}
}