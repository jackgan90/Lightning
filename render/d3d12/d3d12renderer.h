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
			D3D12Renderer(const SharedWindowPtr& pContext, const SharedFileSystemPtr& fs);
			~D3D12Renderer()override;
			void SetClearColor(const ColorF& color)override;
			std::uint8_t GetFrameResourceIndex()const override;
		protected:
			void BeginFrame()override;
			void DoFrame()override;
			void EndFrame()override;
		private:
			struct FrameResource
			{
				ComPtr<ID3D12Fence> fence;
				UINT64 fenceValue;
				SharedRenderTargetPtr renderTargets[MAX_RENDER_TARGET_COUNT];
			};
			void WaitForPreviousFrame(bool waitAll);
			void CreateFences();
			ComPtr<ID3D12CommandQueue> m_commandQueue;
			ComPtr<ID3D12GraphicsCommandList> m_commandList;
			HANDLE m_fenceEvent;
			UINT m_currentBackBufferIndex;
			ColorF m_clearColor;
			std::unique_ptr<D3D12DepthStencilBuffer> m_depthStencilBuffer;
			FrameResource m_frameResources[RENDER_FRAME_COUNT];
#ifndef NDEBUG
			ComPtr<ID3D12Debug> m_d3d12Debug;
			ComPtr<IDXGIDebug> m_dxgiDebug;
			void EnableDebugLayer();
			void InitDXGIDebug();
#endif
		};
	}
}