#include "d3d12renderer.h"
#include "d3d12swapchain.h"
#include "d3d12device.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12rendertarget.h"
#include "d3d12renderfence.h"
#include "winwindow.h"
#include "configmanager.h"
#include "logger.h"
#include "common.h"
#include "ringallocator.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using WindowSystem::WinWindow;
		using Foundation::RingAllocator;

		extern RingAllocator g_RenderAllocator;

		D3D12Renderer::~D3D12Renderer()
		{
			WaitForPreviousFrame(true);
			ReleasePreviousFrameResources(false);
			m_outputWindow.reset();
			logger.Log(LogLevel::Info, "Start to clean up render resources.");
			//Note:we should release resources in advance to make REPORT_LIVE_OBJECTS work correctly because if we let the share pointer
			//destructor run out of the scope,we cannot trace the objects 
			//device , swap chain and depth stencil buffer are parent class's members but we still release them here because we need to track alive resources
			m_device.reset();
			m_swapChain.reset();
			m_depthStencilBuffer.reset();
			D3D12RenderTargetManager::Instance()->Clear();
			D3D12DescriptorHeapManager::Instance()->Clear();
			REPORT_LIVE_OBJECTS;
		}

		D3D12Renderer::D3D12Renderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs) : Renderer(fs), m_outputWindow(pWindow)
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
			auto d3d12Device = static_cast<D3D12Device*>(m_device.get());
			auto pNativeDevice = d3d12Device->GetNative();
			auto commandQueue = GetCommandQueue();
			m_swapChain = std::make_unique<D3D12SwapChain>(dxgiFactory.Get(), pNativeDevice, commandQueue, pWindow.get());
			m_depthStencilBuffer = std::make_shared<D3D12DepthStencilBuffer>(pWindow->GetWidth(), pWindow->GetHeight());
			
			CreateFences();
			logger.Log(LogLevel::Info, "Initialize D3D12 render context succeeded!");
			
			m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
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
		ID3D12CommandQueue* D3D12Renderer::GetCommandQueue()
		{
			return static_cast<D3D12Device*>(m_device.get())->GetCommandQueue();
		}

		ID3D12GraphicsCommandList* D3D12Renderer::GetGraphicsCommandList()
		{
			return static_cast<D3D12Device*>(m_device.get())->GetGraphicsCommandList();
		}

		void D3D12Renderer::CreateFences()
		{
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			D3D12Device* pD3D12Device = static_cast<D3D12Device*>(m_device.get());
			auto nativeDevice = pD3D12Device->GetNative();
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				m_frameResources[i].fence = new D3D12RenderFence(pD3D12Device, 0);
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
			Renderer::BeginFrame();
			ReleasePreviousFrameResources(true);
		}

		void D3D12Renderer::DoFrame()
		{
			Renderer::DoFrame();
		}

		void D3D12Renderer::EndFrame()
		{
			auto currentSwapChainRT = m_swapChain->GetBufferRenderTarget(m_currentBackBufferIndex);
			auto nativeRT = static_cast<D3D12RenderTarget*>(currentSwapChainRT.get());
			auto commandList = GetGraphicsCommandList();
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRT->GetNative().Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
			commandList->Close();
			ID3D12CommandList* commandListArray[] = { commandList, };
			auto commandQueue = GetCommandQueue();
			commandQueue->ExecuteCommandLists(_countof(commandListArray), commandListArray);
			auto fence = m_frameResources[m_currentBackBufferIndex].fence;
			fence->SetTargetValue(fence->GetTargetValue() + 1);
			m_swapChain->Present();
			Renderer::EndFrame();
		}

		void D3D12Renderer::ReleasePreviousFrameResources(bool perFrame)
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
				auto commandQueue = GetCommandQueue();
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