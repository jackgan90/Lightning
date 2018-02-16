//#include "rendererfactory.h"
#include "d3d12renderer.h"
#include "d3d12swapchain.h"
#include "d3d12device.h"
#include "d3d12descriptorheapmanager.h"
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
			m_fences.clear();
			::CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
			//Note:we should release resources in advance to make REPORT_LIVE_OBJECTS work correctly because if we let the share pointer
			//destructor run out of the scope,we cannot trace the objects 
			m_descriptorMgr.reset();
			m_rtMgr.reset();
			m_swapChain.reset();
			m_device.reset();
			REPORT_LIVE_OBJECTS;
		}

		D3D12Renderer::D3D12Renderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs) : Renderer(fs)
			,m_clearColor(0.5f, 0.5f, 0.5f, 1.0f)
		{
#ifndef NDEBUG
			EnableDebugLayer();
#endif
			ComPtr<IDXGIFactory4> dxgiFactory;
			HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create DXGI factory!");
			}
			InitDevice(dxgiFactory);
			m_descriptorMgr = std::make_unique<D3D12DescriptorHeapManager>(static_cast<D3D12Device*>(m_device.get())->m_device);
			m_rtMgr = std::make_unique<D3D12RenderTargetManager>(static_cast<D3D12Device*>(m_device.get()));
			InitSwapChain(dxgiFactory, pWindow);
			
			CreateFences();
			logger.Log(LogLevel::Info, "Initialize D3D12 render context succeeded!");
			
			ComPtr<IDXGISwapChain3> nativeSwapChain = m_swapChain->m_swapChain;
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


		void D3D12Renderer::InitDevice(const ComPtr<IDXGIFactory4>& dxgiFactory)
		{
			ComPtr<IDXGIAdapter1> adaptor;
			int adaptorIndex = 0;
			bool adaptorFound = false;
			HRESULT hr;

			while (dxgiFactory->EnumAdapters1(adaptorIndex, &adaptor) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 desc;
				adaptor->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					adaptorIndex++;
					continue;
				}
				hr = D3D12CreateDevice(adaptor.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(hr))
				{
					adaptorFound = true;
					break;
				}
				adaptorIndex++;
			}
			if (!adaptorFound)
			{
				throw DeviceInitException("Can't find hardware d3d12 adaptor!");
			}
			ComPtr<ID3D12Device> pDevice;
			hr = D3D12CreateDevice(adaptor.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create d3d12 device!");
			}
			m_device = std::make_unique<D3D12Device>(pDevice, m_fs);
		}

		void D3D12Renderer::InitSwapChain(const ComPtr<IDXGIFactory4>& dxgiFactory, const SharedWindowPtr& pWindow)
		{
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			UINT sampleCount = 1;
			bool msaaEnabled = false;
			UINT qualityLevels = 0;
			D3D12Device* pD3D12Device = static_cast<D3D12Device*>(m_device.get());
			if (config.MSAAEnabled)
			{
				D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
				msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
				msQualityLevels.SampleCount = config.MSAASampleCount > 0 ? config.MSAASampleCount : 1;
				msQualityLevels.NumQualityLevels = 0;
				pD3D12Device->m_device->CheckFeatureSupport(
					D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels));
				qualityLevels = msQualityLevels.NumQualityLevels;
				sampleCount = msQualityLevels.SampleCount;
				assert(qualityLevels > 0 && "Unexpected MSAA quality levels.");
			}
			DXGI_MODE_DESC bufferDesc = {};
			bufferDesc.Width = pWindow->GetWidth();
			bufferDesc.Height = pWindow->GetHeight();
			bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			DXGI_SAMPLE_DESC sampleDesc = {};
			sampleDesc.Count = msaaEnabled ? sampleCount : 1;
			sampleDesc.Quality = msaaEnabled ? qualityLevels - 1 : 0;

			DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
			swapChainDesc.BufferCount = config.SwapChainBufferCount;
			swapChainDesc.BufferDesc = bufferDesc;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			auto hwnd = STATIC_CAST_PTR(WinWindow, pWindow)->GetWindowHandle();
			swapChainDesc.OutputWindow = hwnd;
			swapChainDesc.SampleDesc = sampleDesc;
			swapChainDesc.Windowed = TRUE;

			ComPtr<IDXGISwapChain> tempSwapChain;

			dxgiFactory->CreateSwapChain(pD3D12Device->m_commandQueue.Get(),
				&swapChainDesc, &tempSwapChain);
			ComPtr<IDXGISwapChain3> swapChain;
			tempSwapChain.As(&swapChain);
			m_swapChain = std::make_unique<D3D12SwapChain>(swapChain, this);
		}

		void D3D12Renderer::CreateFences()
		{
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			HRESULT hr;
			D3D12Device* pD3D12Device = static_cast<D3D12Device*>(m_device.get());
			ComPtr<ID3D12Device> nativeDevice = pD3D12Device->m_device;
			for (size_t i = 0; i < config.SwapChainBufferCount; i++)
			{
				m_fenceValues.push_back(0);
				ComPtr<ID3D12Fence> fence;
				hr = nativeDevice->CreateFence(m_fenceValues.back(), D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
				if (FAILED(hr))
				{
					throw DeviceInitException("Failed to create fence!");
				}
				m_fences.push_back(fence);
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
			m_frameIndex++;
			m_currentBackBufferIndex = m_swapChain->m_swapChain->GetCurrentBackBufferIndex();
			D3D12Device* pD3D12Device = static_cast<D3D12Device*>(m_device.get());
			auto commandAllocator = pD3D12Device->m_commandAllocators[m_currentBackBufferIndex];
			auto commandList = pD3D12Device->m_commandList;
			commandAllocator->Reset();
			commandList->Reset(commandAllocator.Get(), nullptr);
		}

		void D3D12Renderer::DoFrame()
		{
			//here goes rendering commands
			auto currentSwapChainRT = m_swapChain->GetBufferRenderTarget(m_currentBackBufferIndex);
			m_device->ClearRenderTarget(currentSwapChainRT.get(), m_clearColor);
		}

		void D3D12Renderer::EndFrame()
		{
			D3D12Device* pD3D12Device = static_cast<D3D12Device*>(m_device.get());
			auto commandList = pD3D12Device->m_commandList;
			auto commandQueue = pD3D12Device->m_commandQueue;
			commandList->Close();
			ID3D12CommandList* commandListArray[] = { commandList.Get() };
			commandQueue->ExecuteCommandLists(_countof(commandListArray), commandListArray);
			commandQueue->Signal(m_fences[m_currentBackBufferIndex].Get(), m_fenceValues[m_currentBackBufferIndex]);
			m_swapChain->Present();
		}

		D3D12RenderTargetManager* D3D12Renderer::GetRenderTargetManager()
		{
			return m_rtMgr.get();
		}


		void D3D12Renderer::WaitForPreviousFrame(bool waitAll)
		{
			HRESULT hr;
			ComPtr<IDXGISwapChain3> nativeSwapChain = m_swapChain->m_swapChain;
			std::vector<UINT> bufferIndice;
			if (!waitAll)
			{
				bufferIndice.push_back(nativeSwapChain->GetCurrentBackBufferIndex());
			}
			else
			{
				auto commandQueue = static_cast<D3D12Device*>(m_device.get())->m_commandQueue;
				for (std::size_t i = 0;i < m_swapChain->GetBufferCount();++i)
				{
					bufferIndice.push_back(i);
					//explicit signal to prevent release assert
					commandQueue->Signal(m_fences[i].Get(), m_fenceValues[i]);
				}
			}
			for (const auto& bufferIndex : bufferIndice)
			{
				if (m_fences[bufferIndex]->GetCompletedValue() < m_fenceValues[bufferIndex])
				{
					hr = m_fences[bufferIndex]->SetEventOnCompletion(m_fenceValues[bufferIndex], m_fenceEvent);
					if (FAILED(hr))
					{
						logger.Log(LogLevel::Error, "Failed to SetEventOnCompletion, back buffer index:%d, fence value:%d",
							bufferIndex, m_fenceValues[bufferIndex]);
					}
					::WaitForSingleObject(m_fenceEvent, INFINITE);
				}
				++m_fenceValues[bufferIndex];
			}
		}

		void D3D12Renderer::SetClearColor(const ColorF& color)
		{
			m_clearColor = color;
		}

	}
}