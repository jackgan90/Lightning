//#include "rendererfactory.h"
#include "d3d12renderer.h"
#include "d3d12swapchain.h"
#include "d3d12device.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12rendertarget.h"
#include "winwindow.h"
#include "configmanager.h"
#include "logger.h"
#include "common.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using WindowSystem::WinWindow;

		D3D12Renderer::~D3D12Renderer()
		{
			WaitForPreviousFrame(true);
			logger.Log(LogLevel::Info, "Start to clean up render resources.");
			::CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
			//Note:we should release resources in advance to make REPORT_LIVE_OBJECTS work correctly because if we let the share pointer
			//destructor run out of the scope,we cannot trace the objects 
			m_swapChain.reset();
			m_depthStencilBuffer.reset();
			m_commandQueue.Reset();
			m_commandList.Reset();
			ReleaseFrameResources();
			D3D12RenderTargetManager::Instance()->Clear();
			m_device.reset();
			D3D12DescriptorHeapManager::Instance()->Clear();
			REPORT_LIVE_OBJECTS;
		}

		D3D12Renderer::D3D12Renderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs) : Renderer(fs)
			,m_clearColor(0.5f, 0.5f, 0.5f, 1.0f), m_currentBackBufferIndex(0)
		{
			ComPtr<IDXGIFactory4> dxgiFactory;
#ifndef NDEBUG
			EnableDebugLayer();
			HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory));
#else
			HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
#endif
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create DXGI factory!");
			}
			m_device = std::make_unique<D3D12Device>(dxgiFactory.Get(), m_fs);
			auto pNativeDevice = static_cast<D3D12Device*>(m_device.get())->GetNative();
			m_commandQueue = static_cast<D3D12Device*>(m_device.get())->GetCommandQueue();
			m_commandList = static_cast<D3D12Device*>(m_device.get())->GetGraphicsCommandList();
			m_swapChain = std::make_unique<D3D12SwapChain>(dxgiFactory.Get(), pNativeDevice, m_commandQueue.Get(), pWindow.get());
			m_depthStencilBuffer = std::make_unique<D3D12DepthStencilBuffer>(pWindow->GetWidth(), pWindow->GetHeight());
			
			CreateFences();
			logger.Log(LogLevel::Info, "Initialize D3D12 render context succeeded!");
			
			auto nativeSwapChain = static_cast<D3D12SwapChain*>(m_swapChain.get())->GetNative();
			m_currentBackBufferIndex = nativeSwapChain->GetCurrentBackBufferIndex();
#ifndef NDEBUG
			InitDXGIDebug();
#endif
			REPORT_LIVE_OBJECTS;
		}

#ifndef NDEBUG
		void D3D12Renderer::EnableDebugLayer()
		{
			auto res = ::D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug));
			if (FAILED(res))
			{
				logger.Log(LogLevel::Warning, "Failed to get d3d12 debug interface!You should enable Graphics Tools optional feature!ErrorCode : 0x%x", res);
			}
			else
			{
				m_d3d12Debug->EnableDebugLayer();
			}
		}
#endif

		void D3D12Renderer::CreateFences()
		{
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			HRESULT hr;
			D3D12Device* pD3D12Device = static_cast<D3D12Device*>(m_device.get());
			auto nativeDevice = pD3D12Device->GetNative();
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				m_frameResources[i].fenceValue = 0;
				ComPtr<ID3D12Fence> fence;
				hr = nativeDevice->CreateFence(m_frameResources[i].fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameResources[i].fence));
				if (FAILED(hr))
				{
					throw DeviceInitException("Failed to create fence!");
				}
			}
			m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (!m_fenceEvent)
			{
				throw DeviceInitException("Failed to create fence event!");
			}
		}

#ifndef NDEBUG
		void D3D12Renderer::InitDXGIDebug()
		{
			//HMODULE dxgiDebugHandle = ::GetModuleHandle("Dxgidebug.dll");
			//don't use GetModuleHandle because Dxgidebug.dll may not be loaded automatically by the app
			//so we just load the dll here explicitly
			HMODULE dxgiDebugHandle = ::LoadLibrary("Dxgidebug.dll");
			if (!dxgiDebugHandle)
			{
				logger.Log(LogLevel::Warning, "Can't get dxgidebug.dll module,errorCode:0x%x", ::GetLastError());
				return;
			}
			//the __stdcall declaration is required because windows APIs conform to stdcall convention
			//omit it will cause stack corruption
			using DXGIGetDebugInterfaceFunc = LRESULT (__stdcall *)(REFIID, void**);
			DXGIGetDebugInterfaceFunc pDXGIGetDebugInterface = reinterpret_cast<DXGIGetDebugInterfaceFunc>(::GetProcAddress(dxgiDebugHandle, "DXGIGetDebugInterface"));
			if (!pDXGIGetDebugInterface)
			{
				logger.Log(LogLevel::Warning, "Failed to get debug interface!");
				return;
			}
			pDXGIGetDebugInterface(IID_PPV_ARGS(&m_dxgiDebug));
		}
#endif

		void D3D12Renderer::BeginFrame()
		{
			WaitForPreviousFrame(false);
			m_frameCount++;
			m_currentBackBufferIndex = static_cast<D3D12SwapChain*>(m_swapChain.get())->GetNative()->GetCurrentBackBufferIndex();
			//Release frame resources used by previous frame
			m_frameResources[m_currentBackBufferIndex].Release(true);
			D3D12Device* pD3D12Device = static_cast<D3D12Device*>(m_device.get());
			pD3D12Device->BeginFrame(m_currentBackBufferIndex);
		}

		void D3D12Renderer::DoFrame()
		{
			auto currentSwapChainRT = m_swapChain->GetBufferRenderTarget(m_currentBackBufferIndex);
			m_device->ClearRenderTarget(currentSwapChainRT.get(), m_clearColor);
			static_cast<D3D12Device*>(m_device.get())->ApplyRenderTargets(&currentSwapChainRT, 
				1, m_depthStencilBuffer.get());
		}

		void D3D12Renderer::EndFrame()
		{
			auto currentSwapChainRT = m_swapChain->GetBufferRenderTarget(m_currentBackBufferIndex);
			auto nativeRT = static_cast<D3D12RenderTarget*>(currentSwapChainRT.get());
			m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRT->GetNative().Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
			m_commandList->Close();
			ID3D12CommandList* commandListArray[] = { m_commandList.Get() };
			m_commandQueue->ExecuteCommandLists(_countof(commandListArray), commandListArray);
			m_commandQueue->Signal(m_frameResources[m_currentBackBufferIndex].fence.Get(), 
				m_frameResources[m_currentBackBufferIndex].fenceValue);
			m_swapChain->Present();
		}

		void D3D12Renderer::WaitForPreviousFrame(bool waitAll)
		{
			HRESULT hr;
			auto nativeSwapChain = static_cast<D3D12SwapChain*>(m_swapChain.get())->GetNative();
			std::vector<UINT> bufferIndice;
			if (!waitAll)
			{
				bufferIndice.push_back(nativeSwapChain->GetCurrentBackBufferIndex());
			}
			else
			{
				for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					bufferIndice.push_back(i);
					//explicit signal to prevent release assert
					m_commandQueue->Signal(m_frameResources[i].fence.Get(), m_frameResources[i].fenceValue);
				}
			}
			for (const auto& bufferIndex : bufferIndice)
			{
				if (m_frameResources[bufferIndex].fence->GetCompletedValue() < m_frameResources[bufferIndex].fenceValue)
				{
					hr = m_frameResources[bufferIndex].fence->SetEventOnCompletion(m_frameResources[bufferIndex].fenceValue, m_fenceEvent);
					if (FAILED(hr))
					{
						logger.Log(LogLevel::Error, "Failed to SetEventOnCompletion, back buffer index:%d, fence value:%d",
							bufferIndex, m_frameResources[bufferIndex].fenceValue);
					}
					::WaitForSingleObject(m_fenceEvent, INFINITE);
				}
				++m_frameResources[bufferIndex].fenceValue;
			}
		}

		void D3D12Renderer::SetClearColor(const ColorF& color)
		{
			m_clearColor = color;
		}

		std::uint8_t D3D12Renderer::GetFrameResourceIndex()const
		{
			return m_currentBackBufferIndex;
		}

		void D3D12Renderer::ReleaseFrameResources()
		{
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				m_frameResources[i].Release(false);
			}
		}

	}
}