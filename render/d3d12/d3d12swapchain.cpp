#include <d3d12.h>
#include <dxgi.h>
#include "common.h"
#include "logger.h"
#include "winwindow.h"
#include "d3d12renderer.h"
#include "d3d12swapchain.h"
#include "d3d12rendertarget.h"
#include "d3d12rendertargetmanager.h"
#include "d3d12descriptorheapmanager.h"
#include "configmanager.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using WindowSystem::WinWindow;
		D3D12SwapChain::D3D12SwapChain(IDXGIFactory4* factory, ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue, IWindow* pWindow)
		{
			CreateNativeSwapChain(factory, pDevice, pCommandQueue, pWindow);
			m_swapChain->GetDesc(&m_desc);
			BindRenderTargets(pDevice);
		}

		void D3D12SwapChain::CreateNativeSwapChain(IDXGIFactory4* factory, ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue, IWindow* pWindow)
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
				pDevice->CheckFeatureSupport(
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
			swapChainDesc.BufferCount = RENDER_FRAME_COUNT;
			swapChainDesc.BufferDesc = bufferDesc;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			auto hwnd = static_cast<WinWindow*>(pWindow)->GetWindowHandle();
			swapChainDesc.OutputWindow = hwnd;
			swapChainDesc.SampleDesc = sampleDesc;
			swapChainDesc.Windowed = TRUE;

			ComPtr<IDXGISwapChain> tempSwapChain;

			auto hr = factory->CreateSwapChain(pCommandQueue, &swapChainDesc, &tempSwapChain);
			ComPtr<IDXGISwapChain3> swapChain;
			tempSwapChain.As(&m_swapChain);
		}

		D3D12SwapChain::~D3D12SwapChain()
		{
			m_renderTargets.clear();
		}

		bool D3D12SwapChain::Present()
		{
			HRESULT hr = m_swapChain->Present(0, 0);
			return SUCCEEDED(hr);
		}

		void D3D12SwapChain::BindRenderTargets(ID3D12Device* pDevice)
		{
			HRESULT hr;
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.NumDescriptors = RENDER_FRAME_COUNT;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			const HeapAllocationInfo* pHeapInfo = D3D12DescriptorHeapManager::Instance()->Create(desc);
			if (!pHeapInfo)
			{
				throw SwapChainInitException("Failed to allocate RTV descriptor heap!");
			}
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pHeapInfo->cpuHeapStart);

			ComPtr<ID3D12Resource> swapChainTargets[RENDER_FRAME_COUNT];
			auto rtMgr = D3D12RenderTargetManager::Instance();
			for (int i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainTargets[i]));
				if (FAILED(hr))
				{
					throw SwapChainInitException("Failed to get d3d12 swap chain buffer.");
				}
				pDevice->CreateRenderTargetView(swapChainTargets[i].Get(), nullptr, rtvHandle);
				auto rt = rtMgr->CreateSwapChainRenderTarget(swapChainTargets[i], rtvHandle, this);
				m_renderTargets[i] = rt->GetID();
				rtvHandle.Offset(pHeapInfo->incrementSize);
			}
		}

		SharedRenderTargetPtr D3D12SwapChain::GetBufferRenderTarget(unsigned int bufferIndex)
		{
			auto it = m_renderTargets.find(bufferIndex);
			if (it == m_renderTargets.end())
				return SharedRenderTargetPtr();
			return D3D12RenderTargetManager::Instance()->GetRenderTarget(it->second);
		}


	}
}