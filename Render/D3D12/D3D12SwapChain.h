#pragma once
#include <d3dx12.h>
#include <DXGI.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <unordered_map>
#include "ISwapChain.h"
#include "IWindow.h"
#include "D3D12RenderTargetManager.h"
#include "D3D12TypeMapper.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class D3D12Renderer;
		class D3D12SwapChain : public ISwapChain
		{
		public:
			//we have to use raw pointer,because at the time the swap chain is created, D3D12Renderer is not constructed successfully yet
			//so there's actually no shared pointer pointed to it.Passing a smart pointer here will cause error
			D3D12SwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, Window::IWindow* pWindow);
			~D3D12SwapChain()override;
			bool Present()override;
			std::uint32_t GetSampleCount()const override { return mDesc.SampleDesc.Count; }
			int GetSampleQuality()const override { return mDesc.SampleDesc.Quality; }
			RenderFormat GetRenderFormat()const override{ return D3D12TypeMapper::MapRenderFormat(mDesc.BufferDesc.Format); }
			std::uint32_t GetCurrentBackBufferIndex()const override;
			SharedRenderTargetPtr GetDefaultRenderTarget()override;
		private:
			void BindRenderTargets();
			void CreateNativeSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* pCommandQueue, Window::IWindow* pWindow);
			ComPtr<IDXGISwapChain3> mSwapChain;
			RenderTargetID mRenderTargets[RENDER_FRAME_COUNT];
			DXGI_SWAP_CHAIN_DESC mDesc;
		};
	}
}
