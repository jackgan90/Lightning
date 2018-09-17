#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#ifndef NDEBUG
#include <dxgidebug.h>
#endif
#include "iwindow.h"
#include "irenderfence.h"
#include "renderer.h"
#include "filesystem.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12swapchain.h"
#include "d3d12rendertargetmanager.h"
#include "d3d12depthstencilbuffer.h"

#ifndef NDEBUG
#define REPORT_LIVE_OBJECTS if(mDXGIDebug) {mDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);}
#else
#define REPORT_LIVE_OBJECTS
#endif

namespace Lightning
{
	namespace Render
	{
		using WindowSystem::SharedWindowPtr;
		using Microsoft::WRL::ComPtr;
		class LIGHTNING_RENDER_API D3D12Renderer : public Renderer
		{
		public:
			D3D12Renderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs);
			~D3D12Renderer()override;
			float GetNDCNearPlane()const override { return 0.0f; }
			void ShutDown()override;
			void Start()override;
		protected:
			void BeginFrame()override;
			void DoFrame()override;
			void EndFrame()override;
			IRenderFence* CreateRenderFence()override;
			IDevice* CreateDevice()override;
			ISwapChain* CreateSwapChain()override;
			IDepthStencilBuffer* CreateDepthStencilBuffer(std::size_t width, std::size_t height)override;
		private:
			ID3D12CommandQueue* GetCommandQueue();
			ID3D12GraphicsCommandList* GetGraphicsCommandList();
			ComPtr<IDXGIFactory4> mDXGIFactory;
#ifndef NDEBUG
			ComPtr<ID3D12Debug> mD3D12Debug;
			ComPtr<IDXGIDebug> mDXGIDebug;
			void EnableDebugLayer();
			void InitDXGIDebug();
#endif
		};
	}
}
