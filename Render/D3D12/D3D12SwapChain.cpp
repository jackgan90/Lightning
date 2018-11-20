#include <d3d12.h>
#include <dxgi.h>
#include "Common.h"
#include "Logger.h"
#include "WindowsGameWindow.h"
#include "Renderer.h"
#include "D3D12Renderer.h"
#include "D3D12SwapChain.h"
#include "D3D12RenderTarget.h"
#include "D3D12RenderTargetManager.h"
#include "D3D12DescriptorHeapManager.h"
#include "IPluginMgr.h"
#include "FoundationPlugin.h"

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginMgr* gPluginMgr;
	}

	namespace Render
	{
		using Foundation::EngineConfig;
		using Window::GameWindow;
		using Window::WindowsGameWindow;
		D3D12SwapChain::D3D12SwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* pCommandQueue, Window::IWindow* window)
		{
			CreateNativeSwapChain(factory, pCommandQueue, window);
			mSwapChain->GetDesc(&mDesc);
			BindRenderTargets();
		}

		void D3D12SwapChain::CreateNativeSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* pCommandQueue, Window::IWindow* pWindow)
		{
			auto foundationPlugin = Plugins::gPluginMgr->GetPlugin<Plugins::FoundationPlugin>("Foundation");
			const auto& config = foundationPlugin->GetConfigManager()->GetConfig();
			UINT sampleCount = 1;
			bool msaaEnabled = false;
			UINT qualityLevels = 0;
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			if (config.MSAAEnabled)
			{
				D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
				msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
				msQualityLevels.SampleCount = config.MSAASampleCount > 0 ? config.MSAASampleCount : 1;
				msQualityLevels.NumQualityLevels = 0;
				device->CheckFeatureSupport(
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
			auto hwnd = static_cast<WindowsGameWindow*>(pWindow)->GetWindowHandle();
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

		void D3D12SwapChain::BindRenderTargets()
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
				auto statefulResource = std::make_shared<D3D12StatefulResource>(resources[i], D3D12_RESOURCE_STATE_PRESENT);
				auto renderTarget = rtMgr->CreateSwapChainRenderTarget(statefulResource, this);
				mRenderTargets[i] = renderTarget->GetID();
				renderTarget->Release();
			}
		}

		std::uint32_t D3D12SwapChain::GetCurrentBackBufferIndex()const
		{
			return mSwapChain->GetCurrentBackBufferIndex();
		}

		IRenderTarget* D3D12SwapChain::GetDefaultRenderTarget()
		{
			return D3D12RenderTargetManager::Instance()->GetRenderTarget(mRenderTargets[GetCurrentBackBufferIndex()]);
		}

	}
}