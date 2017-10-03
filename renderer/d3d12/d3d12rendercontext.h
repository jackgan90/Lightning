#pragma once
#include "irendercontext.h"
#include "irendertargetmanager.h"
#include "d3d12renderer.h"
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <vector>
#ifdef DEBUG
#include <dxgidebug.h>
#endif


namespace LightningGE
{
	namespace Renderer
	{
#define REPORT_LIVE_OBJECTS if(m_dxgiDebug) {m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);}
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12RenderContext : public IRenderContext
		{
		public:
			friend class D3D12Renderer;
			~D3D12RenderContext()override;
			bool Init(WindowPtr pWindow)override;
			void ReleaseRenderResources()override;
		private:
			bool InitDevice(ComPtr<IDXGIFactory4> dxgiFactory);
			bool InitSwapChain(ComPtr<IDXGIFactory4> dxgiFactory, WindowPtr pWindow);
			bool BindSwapChainRenderTargets();
			bool CreateFences();
			DevicePtr m_device;
			SwapChainPtr m_swapChain;
			RenderTargetManagerPtr m_renderTargetManager;
			ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
			std::vector<ComPtr<ID3D12Fence>> m_fences;
			std::vector<UINT64> m_fenceValues;
			HANDLE m_fenceEvent;
#ifdef DEBUG
			ComPtr<ID3D12Debug> m_d3d12Debug;
			ComPtr<IDXGIDebug> m_dxgiDebug;
			void InitDXGIDebug();
#endif
		};
	}
}