#include "common.h"
#include "winwindow.h"
#include "winwindownativehandle.h"
#include "d3d12rendercontext.h"
#include "d3d12device.h"
#include "logger.h"

namespace LightningGE
{
	using Utility::logger;
	using Utility::LogLevel;
	using WindowSystem::WinWindow;
	namespace Renderer
	{
		D3D12RenderContext::~D3D12RenderContext()
		{

		}

		bool D3D12RenderContext::Init(WindowPtr pWindow)
		{
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
			logger.Log(LogLevel::Info, "Initialize D3D12 render context succeeded!");
			return true;
		}

		bool D3D12RenderContext::InitDevice(ComPtr<IDXGIFactory4> dxgiFactory)
		{

			IDXGIAdapter1* adaptor = nullptr;
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
				hr = D3D12CreateDevice(adaptor, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
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
			hr = D3D12CreateDevice(adaptor, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice));
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
				dynamic_cast<const WindowSystem::WinWindowNativeHandle*>(DYNAMIC_CAST_PTR(WinWindow, pWindow)->GetNativeHandle());
			swapChainDesc.OutputWindow = *const_cast<WindowSystem::WinWindowNativeHandle*>(pNativeHandle);
			swapChainDesc.SampleDesc = sampleDesc;
			swapChainDesc.Windowed = false;


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

	}
}