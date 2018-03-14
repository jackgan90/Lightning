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
		Renderer::Renderer(const SharedFileSystemPtr& fs, RenderPassType renderPassType) :
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


	}
}