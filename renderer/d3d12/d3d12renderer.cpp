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
		D3D12Renderer::D3D12Renderer(RenderContextPtr pContext) : 
			m_context(pContext), m_clearColor(0.5f, 0.5f, 0.5f, 1.0f)
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
			m_frameIndex++;
		}

		void D3D12Renderer::DoRender()
		{
			WaitForPreviousFrame();
			D3D12RenderContext* pContext = static_cast<D3D12RenderContext*>(m_context.get());
			D3D12SwapChain* pSwapChain = static_cast<D3D12SwapChain*>(pContext->m_swapChain.get());
			D3D12Device* pDevice = static_cast<D3D12Device*>(pContext->m_device.get());
			pSwapChain->Present();
			auto commandAllocator = pDevice->m_commandAllocators[m_currentBackBufferIndex];
			auto commandList = pDevice->m_commandList;
			auto commandQueue = pDevice->m_commandQueue;
			commandAllocator->Reset();
			commandList->Reset(commandAllocator.Get(), nullptr);
			m_currentBackBufferIndex = pSwapChain->m_swapChain->GetCurrentBackBufferIndex();
			//here goes rendering commands
			auto currentSwapChainRT = pSwapChain->GetBufferRenderTarget(m_currentBackBufferIndex);
			pDevice->ClearRenderTarget(currentSwapChainRT, m_clearColor);
			commandList->Close();
			ID3D12CommandList* commandListArray[] = { commandList.Get() };
			commandQueue->ExecuteCommandLists(_countof(commandListArray), commandListArray);
			commandQueue->Signal(pContext->m_fences[m_currentBackBufferIndex].Get(), pContext->m_fenceValues[m_currentBackBufferIndex]);
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
			if (pContext->m_fences[m_currentBackBufferIndex]->GetCompletedValue() < pContext->m_fenceValues[m_currentBackBufferIndex])
			{
				hr = pContext->m_fences[m_currentBackBufferIndex]->SetEventOnCompletion(pContext->m_fenceValues[m_currentBackBufferIndex], pContext->m_fenceEvent);
				if (FAILED(hr))
				{
					logger.Log(LogLevel::Error, "Failed to SetEventOnCompletion, current back buffer index:%d, fence value:%d",
						m_currentBackBufferIndex, pContext->m_fenceValues[m_currentBackBufferIndex]);
				}
				::WaitForSingleObject(pContext->m_fenceEvent, INFINITE);
			}
			++pContext->m_fenceValues[m_currentBackBufferIndex];
		}

		void D3D12Renderer::SetClearColor(const ColorF& color)
		{
			m_clearColor = color;
		}


	}
}