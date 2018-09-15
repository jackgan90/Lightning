#include <cassert>
#include "device.h"
#include "renderer.h"
#include "forwardrenderpass.h"
#include "deferedrenderpass.h"
#include "ringallocator.h"

namespace LightningGE
{
	namespace Render
	{
		Renderer* Renderer::s_instance{ nullptr };
		Foundation::RingAllocator g_RenderAllocator;
		Renderer::Renderer(const SharedFileSystemPtr& fs, const SharedWindowPtr& pWindow, RenderPassType renderPassType) :
			m_outputWindow(pWindow),
			m_frameCount(0), m_fs(fs), m_currentBackBufferIndex(0), m_clearColor(0.5f, 0.5f, 0.5f, 1.0f)
		{
			assert(!s_instance);
			s_instance = this;
			AddRenderPass(renderPassType);
		}

		Renderer::~Renderer()
		{
			assert(s_instance == this);
			s_instance = nullptr;
			m_device.reset();
			m_swapChain.reset();
			m_depthStencilBuffer.reset();
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
			ReleasePreviousFrameResources(true);
			m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
			m_frameCount++;
			m_device->BeginFrame(m_currentBackBufferIndex);
		}

		void Renderer::DoFrame()
		{
			auto currentSwapChainRT = m_swapChain->GetBufferRenderTarget(m_currentBackBufferIndex);
			m_device->ClearRenderTarget(currentSwapChainRT, m_clearColor);
			m_device->ClearDepthStencilBuffer(m_depthStencilBuffer, DepthStencilClearFlags::CLEAR_DEPTH | DepthStencilClearFlags::CLEAR_STENCIL,
				m_depthStencilBuffer->GetDepthClearValue(), m_depthStencilBuffer->GetStencilClearValue(), nullptr);
			static_cast<Device*>(m_device.get())->ApplyRenderTargets(&currentSwapChainRT, 1, m_depthStencilBuffer);
		}

		void Renderer::EndFrame()
		{
			auto fence = m_frameResources[m_currentBackBufferIndex].fence;
			fence->SetTargetValue(fence->GetTargetValue() + 1);
			m_swapChain->Present();
			g_RenderAllocator.FinishFrame(m_frameCount);
		}


		void Renderer::SetClearColor(const ColorF& color)
		{
			m_clearColor = color;
		}

		void Renderer::ApplyRenderPass()
		{
			for (auto& pass : m_renderPasses)
			{
				pass->Apply();
			}
		}

		std::uint64_t Renderer::GetCurrentFrameCount()const
		{
			return m_frameCount;
		}

		IDevice* Renderer::GetDevice()
		{
			return m_device.get();
		}

		ISwapChain* Renderer::GetSwapChain()
		{
			return m_swapChain.get();
		}


		void Renderer::Draw(const RenderItem& item)
		{
			for (auto& pass : m_renderPasses)
			{
				pass->Draw(item);
			}
		}

		void Renderer::AddRenderPass(RenderPassType type)
		{
			switch (type)
			{
			case RenderPassType::FORWARD:
				m_renderPasses.emplace_back(std::make_unique<ForwardRenderPass>());
				break;
			case RenderPassType::DEFERED:
				m_renderPasses.emplace_back(std::make_unique<DeferedRenderPass>());
				break;
			default:
				break;
			}
		}

		std::size_t Renderer::GetFrameResourceIndex()const
		{
			return m_currentBackBufferIndex;
		}

		void Renderer::Start()
		{
			m_device.reset(CreateDevice());
			m_swapChain.reset(CreateSwapChain());
			m_depthStencilBuffer.reset(CreateDepthStencilBuffer(m_outputWindow->GetWidth(), m_outputWindow->GetHeight()));
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				m_frameResources[i].fence = CreateRenderFence();
			}
			m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
		}

		void Renderer::ShutDown()
		{
			WaitForPreviousFrame(true);
			ReleasePreviousFrameResources(false);
			m_outputWindow.reset();
			m_device.reset();
			m_swapChain.reset();
			m_depthStencilBuffer.reset();
		}

		void Renderer::ReleasePreviousFrameResources(bool perFrame)
		{
			//trace back RENDER_FRAME_COUNT frames trying to release temporary memory
			auto backBufferIndex = m_currentBackBufferIndex;
			for (std::size_t i = 0; i < RENDER_FRAME_COUNT;++i)
			{
				if (backBufferIndex == 0)
					backBufferIndex = RENDER_FRAME_COUNT - 1;
				else
					backBufferIndex -= 1;
				auto currentVal = m_frameResources[backBufferIndex].fence->GetCurrentValue();
				auto targetVal = m_frameResources[backBufferIndex].fence->GetTargetValue();
				if (currentVal >= targetVal)
				{
					if (m_frameCount > i + 1)
					{
						g_RenderAllocator.ReleaseFramesBefore(m_frameCount - i - 1);
						m_frameResources[backBufferIndex].Release(perFrame);
					}
				}
			}
			if (!perFrame)
			{
				for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					m_frameResources[i].Release(false);
				}
			}
		}


		void Renderer::WaitForPreviousFrame(bool waitAll)
		{
			HRESULT hr;
			std::vector<UINT> bufferIndice;
			if (!waitAll)
			{
				bufferIndice.push_back(m_swapChain->GetCurrentBackBufferIndex());
			}
			else
			{
				for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					bufferIndice.push_back(i);
					//explicit signal to prevent release assert
					m_frameResources[i].fence->SetTargetValue(m_frameResources[i].fence->GetTargetValue() + 1);
				}
			}
			for (const auto& bufferIndex : bufferIndice)
			{
				m_frameResources[bufferIndex].fence->WaitForTarget();
			}
		}


	}
}