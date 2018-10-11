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
		Renderer* Renderer::sInstance{ nullptr };
		Foundation::FrameMemoryAllocator g_RenderAllocator;
		Renderer::Renderer(const SharedFileSystemPtr& fs, const SharedWindowPtr& pWindow, RenderPassType renderPassType) :
			mOutputWindow(pWindow),
			mFrameCount(0), mFs(fs), mCurrentBackBufferIndex(0), mClearColor(0.5f, 0.5f, 0.5f, 1.0f)
		{
			assert(!sInstance);
			sInstance = this;
			AddRenderPass(renderPassType);
		}

		Renderer::~Renderer()
		{
			assert(sInstance == this);
			sInstance = nullptr;
			mDevice.reset();
			mSwapChain.reset();
			mDepthStencilBuffer.reset();
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
			mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
			mDevice->BeginFrame(mCurrentBackBufferIndex);
			INVOKE_CALLBACK(OnBeginFrame)
		}

		void Renderer::DoFrame()
		{
			auto currentSwapChainRT = mSwapChain->GetBufferRenderTarget(mCurrentBackBufferIndex);
			mDevice->ClearRenderTarget(currentSwapChainRT, mClearColor);
			mDevice->ClearDepthStencilBuffer(mDepthStencilBuffer, DepthStencilClearFlags::CLEAR_DEPTH | DepthStencilClearFlags::CLEAR_STENCIL,
				mDepthStencilBuffer->GetDepthClearValue(), mDepthStencilBuffer->GetStencilClearValue(), nullptr);
			static_cast<Device*>(mDevice.get())->ApplyRenderTargets(&currentSwapChainRT, 1, mDepthStencilBuffer);
			INVOKE_CALLBACK(OnDoFrame)
		}

		void Renderer::EndFrame()
		{
			INVOKE_CALLBACK(OnEndFrame)
			auto fence = mFrameResources[mCurrentBackBufferIndex].fence;
			auto fenceValue = fence->GetTargetValue() + 1;
			mFrameResources[mCurrentBackBufferIndex].frame = mFrameCount;
			fence->SetTargetValue(fenceValue);
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


		void Renderer::Draw(const RenderNode& item)
		{
			for (auto& pass : mRenderPasses)
			{
				pass->Draw(item);
			}
		}

		void Renderer::AddRenderPass(RenderPassType type)
		{
			switch (type)
			{
			case RenderPassType::FORWARD:
				mRenderPasses.emplace_back(std::make_unique<ForwardRenderPass>());
				break;
			case RenderPassType::DEFERED:
				mRenderPasses.emplace_back(std::make_unique<DeferedRenderPass>());
				break;
			default:
				break;
			}
		}

		std::size_t Renderer::GetFrameResourceIndex()const
		{
			return mCurrentBackBufferIndex;
		}

		void Renderer::RegisterCallback(IRendererCallback* callback)
		{
			mCallbacks.push_back(callback);
		}

		void Renderer::Start()
		{
			mDevice.reset(CreateDevice());
			mSwapChain.reset(CreateSwapChain());
			mDepthStencilBuffer.reset(CreateDepthStencilBuffer(mOutputWindow->GetWidth(), mOutputWindow->GetHeight()));
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				mFrameResources[i].fence = CreateRenderFence();
			}
			mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
		}

		void Renderer::ShutDown()
		{
			WaitForPreviousFrame(true);
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mFrameResources[i].Release(false);
			}
			mOutputWindow.reset();
			mDevice.reset();
			mSwapChain.reset();
			mDepthStencilBuffer.reset();
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