#include "d3d12renderer.h"
#include "d3d12swapchain.h"
#include "d3d12device.h"
#include "logger.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		D3D12Renderer::D3D12Renderer(RenderContextPtr pContext) :m_context(pContext)
		{
			D3D12RenderContext* pD3D12Context = static_cast<D3D12RenderContext*>(m_context.get());
			D3D12SwapChain* pSwapChain = static_cast<D3D12SwapChain*>(pD3D12Context->m_swapChain.get());
			ComPtr<IDXGISwapChain3> nativeSwapChain = pSwapChain->m_swapChain;
			m_currentBackBufferIndex = nativeSwapChain->GetCurrentBackBufferIndex();
		}

		void D3D12Renderer::ReleaseRenderResources()
		{
			WaitForPreviousFrame();
			m_context->ReleaseRenderResources();
		}

		void D3D12Renderer::BeginRender()
		{

		}

		void D3D12Renderer::DoRender()
		{
			WaitForPreviousFrame();
			D3D12RenderContext* pContext = static_cast<D3D12RenderContext*>(m_context.get());
			D3D12SwapChain* pSwapChain = static_cast<D3D12SwapChain*>(pContext->m_swapChain.get());
			D3D12Device* pDevice = static_cast<D3D12Device*>(pContext->m_device.get());
			auto commandAllocator = pDevice->m_commandAllocators[m_currentBackBufferIndex];
			auto commandList = pDevice->m_commandList;
			commandAllocator->Reset();
			commandList->Reset(commandAllocator.Get(), nullptr);
			pSwapChain->Present();
			commandList->Close();
		}

		void D3D12Renderer::EndRender()
		{
 }

		DevicePtr D3D12Renderer::GetDevice()
		{
			return static_cast<D3D12RenderContext*>(m_context.get())->m_device;
		}

		SwapChainPtr D3D12Renderer::GetSwapChain()
		{
			return static_cast<D3D12RenderContext*>(m_context.get())->m_swapChain;
		}

		void D3D12Renderer::WaitForPreviousFrame()
		{
			HRESULT hr;
			D3D12RenderContext* pContext = static_cast<D3D12RenderContext*>(m_context.get());
			D3D12SwapChain* pSwapChain = static_cast<D3D12SwapChain*>(pContext->m_swapChain.get());
			ComPtr<IDXGISwapChain3> nativeSwapChain = pSwapChain->m_swapChain;
			m_currentBackBufferIndex = nativeSwapChain->GetCurrentBackBufferIndex();
			auto fences = pContext->m_fences;
			auto fenceValues = pContext->m_fenceValues;
			auto fenceEvent = pContext->m_fenceEvent;
			if (fences[m_currentBackBufferIndex]->GetCompletedValue() < fenceValues[m_currentBackBufferIndex])
			{
				hr = fences[m_currentBackBufferIndex]->SetEventOnCompletion(fenceValues[m_currentBackBufferIndex], fenceEvent);
				if (FAILED(hr))
				{
					logger.Log(LogLevel::Error, "Failed to SetEventOnCompletion, current back buffer index:%d, fence value:%d",
						m_currentBackBufferIndex, fenceValues[m_currentBackBufferIndex]);
				}
				::WaitForSingleObject(fenceEvent, INFINITE);
			}
			fenceValues[m_currentBackBufferIndex]++;
		}

	}
}