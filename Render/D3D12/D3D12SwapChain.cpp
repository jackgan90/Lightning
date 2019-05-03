#include <d3d12.h>
#include <dxgi.h>
#include "Common.h"
#include "Logger.h"
#include "WindowsGameWindow.h"
#include "Renderer.h"
#include "D3D12Renderer.h"
#include "D3D12SwapChain.h"
#include "D3D12RenderTarget.h"
#include "IPluginManager.h"
#include "IFoundationPlugin.h"

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginManager* gPluginMgr;
	}

	namespace Render
	{
		using Foundation::EngineConfig;
		using Window::GameWindow;
		using Window::WindowsGameWindow;
		D3D12SwapChain::D3D12SwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* pCommandQueue, Window::IWindow* window)
			: SwapChain(window)
		{
			CreateNativeSwapChain(factory, pCommandQueue, window);
			mSwapChain->GetDesc(&mDesc);
			mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
			CreateRenderTargets();
		}

		D3D12SwapChain::~D3D12SwapChain()
		{
		}

		bool D3D12SwapChain::Present()
		{
			HRESULT hr = mSwapChain->Present(0, 0);
			mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
			return SUCCEEDED(hr);
		}

		std::shared_ptr<IRenderTarget> D3D12SwapChain::GetCurrentRenderTarget()
		{
			return mRenderTargets[mCurrentBackBufferIndex];
		}

		void D3D12SwapChain::Resize(std::size_t width, std::size_t height)
		{
			for (auto renderTarget : mRenderTargets)
			{
				auto D3DRenderTarget = dynamic_cast<D3D12RenderTarget*>(renderTarget.get());
				assert(D3DRenderTarget != nullptr && "A D3DRenderTarget is required.");
				//Release reference to ID3D12Resource
				D3DRenderTarget->Reset();
			}
			auto oldBackBufferIndex = static_cast<std::uint8_t>(mSwapChain->GetCurrentBackBufferIndex());
			mDesc.BufferDesc.Width = static_cast<UINT>(width);
			mDesc.BufferDesc.Height = static_cast<UINT>(height);
			if (mDesc.BufferDesc.Width == 0)
				mDesc.BufferDesc.Width = 8;
			if (mDesc.BufferDesc.Height == 0)
				mDesc.BufferDesc.Height = 8;
			mSwapChain->ResizeBuffers(mDesc.BufferCount, mDesc.BufferDesc.Width, mDesc.BufferDesc.Height, mDesc.BufferDesc.Format, mDesc.Flags);
			//ResizeBuffers may adjust the buffer size to a proper size if width and height
			//have irrational values.For example 0,so we get the desc again
			mSwapChain->GetDesc(&mDesc);

			mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
			if (oldBackBufferIndex != mCurrentBackBufferIndex)
			{
				std::shared_ptr<IRenderTarget> oldRenderTargets[RENDER_FRAME_COUNT];
				for (auto i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					oldRenderTargets[i] = mRenderTargets[i];
				}
				auto i{ 0 };
				for (;i < RENDER_FRAME_COUNT - oldBackBufferIndex;++i)
				{
					mRenderTargets[i] = oldRenderTargets[oldBackBufferIndex + i];
				}
				auto j{ 0 };
				for (;i < RENDER_FRAME_COUNT;++i)
				{
					mRenderTargets[i] = oldRenderTargets[j++];
				}
			}

			for (std::uint8_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				auto D3DRenderTarget = std::static_pointer_cast<D3D12RenderTarget>(mRenderTargets[i]);
				ComPtr<ID3D12Resource> D3DResource;
				auto hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&D3DResource));
				assert(SUCCEEDED(hr) && "Failed to get swap chain buffer.");
				D3DRenderTarget->Reset(D3DResource, D3D12_RESOURCE_STATE_PRESENT);
			}
		}

		void D3D12SwapChain::CreateRenderTargets()
		{
			ComPtr<ID3D12Resource> resources[RENDER_FRAME_COUNT];
			auto device = dynamic_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			assert(device != nullptr && "A D3D12Device is required.");
			for (int i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				auto hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&resources[i]));
				if (FAILED(hr))
				{
					throw SwapChainInitException("Failed to get d3d12 swap chain buffer.");
				}
				auto texture = device->CreateTexture(resources[i], D3D12_RESOURCE_STATE_PRESENT);
				mRenderTargets[i] = device->CreateRenderTarget(texture);
			}
		}

		void D3D12SwapChain::CreateNativeSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* pCommandQueue, Window::IWindow* pWindow)
		{
			auto foundationPlugin = Plugins::GetPlugin<Plugins::IFoundationPlugin>(Plugins::gPluginMgr, "Foundation");
			const auto& config = foundationPlugin->GetConfigManager()->GetConfig();
			UINT sampleCount = 1;
			bool msaaEnabled = false;
			UINT qualityLevels = 0;
			auto device = dynamic_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			assert(device != nullptr && "A D3D12Device is required.");
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
			auto gameWindow = dynamic_cast<WindowsGameWindow*>(pWindow);
			assert(gameWindow != nullptr && "A WindowsGameWindow is required.");
			auto hwnd = gameWindow->GetWindowHandle();
			swapChainDesc.OutputWindow = hwnd;
			swapChainDesc.SampleDesc = sampleDesc;
			swapChainDesc.Windowed = TRUE;

			ComPtr<IDXGISwapChain> tempSwapChain;

			auto hr = factory->CreateSwapChain(pCommandQueue, &swapChainDesc, &tempSwapChain);
			ComPtr<IDXGISwapChain3> swapChain;
			tempSwapChain.As(&mSwapChain);
		}
	}
}