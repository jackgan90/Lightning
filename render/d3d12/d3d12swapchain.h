#pragma once
#include <d3dx12.h>
#include <DXGI.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <unordered_map>
#include "iswapchain.h"
#include "windowsystem.h"
#include "d3d12rendertargetmanager.h"
#include "d3d12typemapper.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Window1::WindowSystem;
		class D3D12Renderer;
		class LIGHTNING_RENDER_API D3D12SwapChain : public ISwapChain
		{
		public:
			//we have to use raw pointer,because at the time the swap chain is created, D3D12Renderer is not constructed successfully yet
			//so there's actually no shared pointer pointed to it.Passing a smart pointer here will cause error
			D3D12SwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, WindowSystem* pWindow);
			~D3D12SwapChain()override;
			bool Present()override;
			std::size_t GetSampleCount()const override { return mDesc.SampleDesc.Count; }
			int GetSampleQuality()const override { return mDesc.SampleDesc.Quality; }
			RenderFormat GetRenderFormat()const override{ return D3D12TypeMapper::MapRenderFormat(mDesc.BufferDesc.Format); }
			std::size_t GetCurrentBackBufferIndex()const override;
			SharedRenderTargetPtr GetDefaultRenderTarget()override;
		private:
			void BindRenderTargets();
			void CreateNativeSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* pCommandQueue, WindowSystem* pWindow);
			ComPtr<IDXGISwapChain3> mSwapChain;
			RenderTargetID mRenderTargets[RENDER_FRAME_COUNT];
			DXGI_SWAP_CHAIN_DESC mDesc;
		};
	}
}
