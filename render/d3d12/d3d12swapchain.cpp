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

namespace Lightning
{
	namespace Render
	{
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using WindowSystem::WinWindow;
		D3D12SwapChain::D3D12SwapChain(IDXGIFactory4* factory, ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue, IWindow* pWindow)
		{
			CreateNativeSwapChain(factory, pDevice, pCommandQueue, pWindow);
			mSwapChain->GetDesc(&mDesc);
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
			tempSwapChain.As(&mSwapChain);
		}

		D3D12SwapChain::~D3D12SwapChain()
		{
		}

		bool D3D12SwapChain::Present()
		{
			HRESULT hr = mSwapChain->Present(0, 0);
			return SUCCEEDED(hr);
		}

		void D3D12SwapChain::BindRenderTargets(ID3D12Device* pDevice)
		{
			ComPtr<ID3D12Resource> resources[RENDER_FRAME_COUNT];
			auto rtMgr = D3D12RenderTargetManager::Instance();
			for (int i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				auto hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&resources[i]));
				if (FAILED(hr))
				{
					throw SwapChainInitException("Failed to get d3d12 swap chain buffer.");
				}
				auto renderTarget = rtMgr->CreateSwapChainRenderTarget(resources[i], pDevice, this);
				mRenderTargets[i] = renderTarget->GetID();
			}
		}

		std::size_t D3D12SwapChain::GetCurrentBackBufferIndex()const
		{
			return mSwapChain->GetCurrentBackBufferIndex();
		}

		SharedRenderTargetPtr D3D12SwapChain::GetDefaultRenderTarget()
		{
			return D3D12RenderTargetManager::Instance()->GetRenderTarget(mRenderTargets[GetCurrentBackBufferIndex()]);
		}

	}
}