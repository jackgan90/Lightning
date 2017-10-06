#include <d3dx12.h>
#include <cassert>
#include "common.h"
#include "winwindow.h"
#include "winwindownativehandle.h"
#include "d3d12rendercontext.h"
#include "d3d12device.h"
#include "d3d12swapchain.h"
#include "rendererfactory.h"
#include "d3d12rendertargetmanager.h"
#include "d3d12rendertarget.h"
#include "d3d12descriptorheapmanager.h"
#include "logger.h"
#include "configmanager.h"

namespace LightningGE
{
	using Foundation::logger;
	using Foundation::LogLevel;
	using Foundation::ConfigManager;
	using Foundation::EngineConfig;
	using WindowSystem::WinWindow;
	using WindowSystem::WindowNativeHandlePtr;
	namespace Renderer
	{
		D3D12RenderContext::~D3D12RenderContext()
		{

		}

		bool D3D12RenderContext::Init(const WindowPtr& pWindow)
		{
#ifdef DEBUG
			::D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug));
			m_d3d12Debug->EnableDebugLayer();
#endif
			ComPtr<IDXGIFactory4> dxgiFactory;
			HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to create DXGI Factory!");
				return false;
			}
			if (!InitDevice(dxgiFactory))
			{
				return false;
			}
			
			if (!InitSwapChain(dxgiFactory, pWindow))
			{
				return false;
			}
			
			m_renderTargetManager = RendererFactory::CreateRenderTargetManager(m_device, m_swapChain);
			D3D12DescriptorHeapManager::CreateInstance(static_cast<D3D12Device*>(m_device.get())->m_device);
			D3D12SwapChain *d3d12swapchain = static_cast<D3D12SwapChain*>(m_swapChain.get());
			if (!d3d12swapchain->BindSwapChainRenderTargets())
			{
				return false;
			}
			if (!CreateFences())
			{
				return false;
			}
			logger.Log(LogLevel::Info, "Initialize D3D12 render context succeeded!");
			
#ifdef DEBUG
			InitDXGIDebug();
#endif
			REPORT_LIVE_OBJECTS;
			return true;
		}

		bool D3D12RenderContext::InitDevice(ComPtr<IDXGIFactory4> dxgiFactory)
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
				logger.Log(LogLevel::Warning, "Can't find hardware d3d12 adaptor!");
				return false;
			}
			ComPtr<ID3D12Device> pDevice;
			hr = D3D12CreateDevice(adaptor.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to create d3d12 device!");
				return false;
			}
			m_device = std::shared_ptr<IDevice>(new D3D12Device(pDevice, this));
			return true;
		}

		bool D3D12RenderContext::InitSwapChain(ComPtr<IDXGIFactory4> dxgiFactory, const WindowPtr& pWindow)
		{
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			UINT sampleCount = 1;
			bool msaaEnabled = false;
			UINT qualityLevels = 0;
			if (config.MSAAEnabled)
			{
				D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
				msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
				msQualityLevels.SampleCount = config.MSAASampleCount > 0 ? config.MSAASampleCount : 1;
				msQualityLevels.NumQualityLevels = 0;
				STATIC_CAST_PTR(D3D12Device, m_device)->m_device->CheckFeatureSupport(
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
			const WindowSystem::WinWindowNativeHandle *pNativeHandle = \
				dynamic_cast<const WindowSystem::WinWindowNativeHandle*>(STATIC_CAST_PTR(WinWindow, pWindow)->GetNativeHandle().get());
			swapChainDesc.OutputWindow = *const_cast<WindowSystem::WinWindowNativeHandle*>(pNativeHandle);
			swapChainDesc.SampleDesc = sampleDesc;
			swapChainDesc.Windowed = TRUE;

			ComPtr<IDXGISwapChain> tempSwapChain;

			dxgiFactory->CreateSwapChain(STATIC_CAST_PTR(D3D12Device, m_device)->m_commandQueue.Get(),
				&swapChainDesc, &tempSwapChain);
			ComPtr<IDXGISwapChain3> swapChain;
			tempSwapChain.As(&swapChain);
			m_swapChain = SwapChainPtr(new D3D12SwapChain(swapChain, static_cast<D3D12Device*>(m_device.get())));
			return true;
		}


		bool D3D12RenderContext::CreateFences()
		{
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			HRESULT hr;
			D3D12Device* pDevice = STATIC_CAST_PTR(D3D12Device, m_device);
			ComPtr<ID3D12Device> pd3d12device = pDevice->m_device;
			for (int i = 0; i < config.SwapChainBufferCount; i++)
			{
				m_fenceValues.push_back(0);
				ComPtr<ID3D12Fence> fence;
				hr = pd3d12device->CreateFence(m_fenceValues.back(), D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
				if (FAILED(hr))
				{
					logger.Log(LogLevel::Error, "Failed to create fence!");
					return false;
				}
				m_fences.push_back(fence);
			}
			m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (!m_fenceEvent)
			{
				logger.Log(LogLevel::Error, "Failed to create fence event!");
				return false;
			}
			return true;
		}



		void D3D12RenderContext::ReleaseRenderResources()
		{
			logger.Log(LogLevel::Info, "Start to clean up render context.");
			m_fences.clear();
			::CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
			m_renderTargetManager->ReleaseRenderResources();
			D3D12DescriptorHeapManager::Instance()->ReleaseRenderResources();
			m_swapChain->ReleaseRenderResources();
			m_device->ReleaseRenderResources();
			REPORT_LIVE_OBJECTS;
		}

#ifdef DEBUG
		void D3D12RenderContext::InitDXGIDebug()
		{
			HMODULE dxgiDebugHandle = ::GetModuleHandle("Dxgidebug.dll");
			typedef LRESULT (*DXGIGetDebugInterfaceFunc)(REFIID, void**);
			DXGIGetDebugInterfaceFunc pDXGIGetDebugInterface = reinterpret_cast<DXGIGetDebugInterfaceFunc>(::GetProcAddress(dxgiDebugHandle, "DXGIGetDebugInterface"));
			pDXGIGetDebugInterface(IID_PPV_ARGS(&m_dxgiDebug));
		}
#endif
	}
}