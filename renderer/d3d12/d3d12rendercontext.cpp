#include <d3dx12.h>
#include "common.h"
#include "winwindow.h"
#include "winwindownativehandle.h"
#include "d3d12rendercontext.h"
#include "d3d12device.h"
#include "d3d12swapchain.h"
#include "rendererfactory.h"
#include "d3d12rendertargetmanager.h"
#include "d3d12rendertarget.h"
#include "logger.h"

namespace LightningGE
{
	using Foundation::logger;
	using Foundation::LogLevel;
	using WindowSystem::WinWindow;
	using WindowSystem::WindowNativeHandlePtr;
	namespace Renderer
	{
		D3D12RenderContext::~D3D12RenderContext()
		{

		}

		bool D3D12RenderContext::Init(WindowPtr pWindow)
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
			
			m_renderTargetManager = RendererFactory::GetRenderTargetManager(m_device, m_swapChain);
			if (!BindSwapChainRenderTargets())
			{
				return false;
			}
			
			logger.Log(LogLevel::Info, "Initialize D3D12 render context succeeded!");
			
#ifdef DEBUG
			InitDXGIDebug();
			m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif
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
			m_device = std::shared_ptr<IDevice>(new D3D12Device(pDevice));
			return true;
		}

		bool D3D12RenderContext::InitSwapChain(ComPtr<IDXGIFactory4> dxgiFactory, WindowPtr pWindow)
		{
			//TODO : init swapchain based on configuration
			DXGI_MODE_DESC bufferDesc = {};
			bufferDesc.Width = pWindow->GetWidth();
			bufferDesc.Height = pWindow->GetHeight();
			bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			DXGI_SAMPLE_DESC sampleDesc = {};
			sampleDesc.Count = 1;

			DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
			swapChainDesc.BufferCount = 3;
			swapChainDesc.BufferDesc = bufferDesc;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			const WindowSystem::WinWindowNativeHandle *pNativeHandle = \
				dynamic_cast<const WindowSystem::WinWindowNativeHandle*>(DYNAMIC_CAST_PTR(WinWindow, pWindow)->GetNativeHandle().get());
			swapChainDesc.OutputWindow = *const_cast<WindowSystem::WinWindowNativeHandle*>(pNativeHandle);
			swapChainDesc.SampleDesc = sampleDesc;
			swapChainDesc.Windowed = TRUE;

			//TODO : is there potential memory leak here?
			ComPtr<IDXGISwapChain> tempSwapChain;

			dxgiFactory->CreateSwapChain(DYNAMIC_CAST_PTR(D3D12Device, m_device)->m_commandQueue.Get(),
				&swapChainDesc, &tempSwapChain);
			ComPtr<IDXGISwapChain3> swapChain;
			tempSwapChain.As(&swapChain);
			m_swapChain = SwapChainPtr(new D3D12SwapChain(swapChain));
			return true;
		}

		bool D3D12RenderContext::BindSwapChainRenderTargets()
		{
			ComPtr<ID3D12Device> pd3ddevice = DYNAMIC_CAST_PTR(D3D12Device, m_device)->m_device;
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			int renderTargetCount = m_swapChain->GetBufferCount();
			desc.NumDescriptors = renderTargetCount;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			HRESULT hr = pd3ddevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvDescriptorHeap));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to create render target descriptor heap!");
				return false;
			}
			UINT descriptorSize = pd3ddevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

			ComPtr<IDXGISwapChain3> pd3dswapchain = DYNAMIC_CAST_PTR(D3D12SwapChain, m_swapChain)->m_swapChain;
			ComPtr<ID3D12Resource>* swapChainTargets = new ComPtr<ID3D12Resource>[renderTargetCount];
			D3D12RenderTargetManager* pRTManager = DYNAMIC_CAST_PTR(D3D12RenderTargetManager, m_renderTargetManager);
			for (int i = 0; i < renderTargetCount; i++)
			{
				hr = pd3dswapchain->GetBuffer(i, IID_PPV_ARGS(&swapChainTargets[i]));
				if (FAILED(hr))
				{
					logger.Log(LogLevel::Error, "Failed to get d3d12 swapchain buffer %d", i);
					delete []swapChainTargets;
					return false;
				}
				pd3ddevice->CreateRenderTargetView(swapChainTargets[i].Get(), nullptr, rtvHandle);
				pRTManager->m_renderTargets[pRTManager->m_currentID++] = RenderTargetPtr(new D3D12RenderTarget(swapChainTargets[i]));
				rtvHandle.Offset(descriptorSize);
			}
			delete []swapChainTargets;
			return true;
		}



		DevicePtr D3D12RenderContext::GetDevice()
		{
			return m_device;
		}

		SwapChainPtr D3D12RenderContext::GetSwapChain()
		{
			return m_swapChain;
		}

		void D3D12RenderContext::Render()
		{

		}

		void D3D12RenderContext::ReleaseRenderResources()
		{
			logger.Log(LogLevel::Info, "Start to clean up render context.");
			m_device->ReleaseRenderResources();
			m_swapChain->ReleaseRenderResources();
			m_renderTargetManager->ReleaseRenderResources();
			m_rtvDescriptorHeap.Reset();
#ifdef DEBUG
			m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif // DEBUG
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