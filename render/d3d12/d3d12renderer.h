#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#ifndef NDEBUG
#include <dxgidebug.h>
#endif
#include "iwindow.h"
#include "renderer.h"
#include "filesystem.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12swapchain.h"
#include "d3d12rendertargetmanager.h"
#include "d3d12depthstencilbuffer.h"

#ifndef NDEBUG
#define REPORT_LIVE_OBJECTS if(m_dxgiDebug) {m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);}
#else
#define REPORT_LIVE_OBJECTS
#endif

namespace LightningGE
{
	namespace Render
	{
		using WindowSystem::SharedWindowPtr;
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDER_API D3D12Renderer : public Renderer
		{
		public:
			LIGHTNINGGE_ALIGNED_OPERATOR_NEW
			D3D12Renderer(const SharedWindowPtr& pContext, const SharedFileSystemPtr& fs);
			~D3D12Renderer()override;
			D3D12RenderTargetManager* GetRenderTargetManager();
			void SetClearColor(const ColorF& color)override;
		protected:
			void BeginFrame()override;
			void DoFrame()override;
			void EndFrame()override;
		private:
			void WaitForPreviousFrame(bool waitAll);
			void InitDevice(const ComPtr<IDXGIFactory4>& dxgiFactory);
			void InitSwapChain(const ComPtr<IDXGIFactory4>& dxgiFactory, const SharedWindowPtr& pWindow);
			void CreateFences();
			std::unique_ptr<D3D12SwapChain> m_swapChain;
			std::unique_ptr<D3D12RenderTargetManager> m_rtMgr;
			std::vector<ComPtr<ID3D12Fence>> m_fences;
			std::vector<UINT64> m_fenceValues;
			HANDLE m_fenceEvent;
			UINT m_currentBackBufferIndex;
			ColorF m_clearColor;
			std::unique_ptr<D3D12DepthStencilBuffer> m_depthStencilBuffer;
			RenderTargetList m_renderTargets;
#ifndef NDEBUG
			ComPtr<ID3D12Debug> m_d3d12Debug;
			ComPtr<IDXGIDebug> m_dxgiDebug;
			void EnableDebugLayer();
			void InitDXGIDebug();
#endif
		};
	}
}