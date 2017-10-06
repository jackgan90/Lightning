#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <vector>
#include <unordered_map>
#ifdef DEBUG
#include <dxgidebug.h>
#endif
#include "irendercontext.h"
#include "irendertargetmanager.h"



namespace LightningGE
{
	namespace Renderer
	{
#ifdef DEBUG
#define REPORT_LIVE_OBJECTS if(m_dxgiDebug) {m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);}
#else
#define REPORT_LIVE_OBJECTS
#endif
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12RenderContext : public IRenderContext
		{
		public:
			friend class D3D12Renderer;
			friend class D3D12Device;
			~D3D12RenderContext()override;
			bool Init(const WindowPtr& pWindow)override;
			void ReleaseRenderResources()override;
		private:
			bool InitDevice(ComPtr<IDXGIFactory4> dxgiFactory);
			bool InitSwapChain(ComPtr<IDXGIFactory4> dxgiFactory, const WindowPtr& pWindow);
			bool CreateFences();
			DevicePtr m_device;
			SwapChainPtr m_swapChain;
			RenderTargetManagerPtr m_renderTargetManager;
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