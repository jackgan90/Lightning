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
			D3D12Renderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs);
			~D3D12Renderer()override;
			IWindow* GetOutputWindow()override { return m_outputWindow.get(); }
			float GetNDCNearPlane()const override { return 0.0f; }
		protected:
			void BeginFrame()override;
			void DoFrame()override;
			void EndFrame()override;
		private:
			struct FrameResource
			{
				IRenderFence *fence;
				void Release(bool perFrame)
				{
					if (!perFrame)
					{
						if (fence)
						{
							delete fence;
							fence = nullptr;
						}
					}
				}
			};
			void WaitForPreviousFrame(bool waitAll);
			void ReleasePreviousFrameResources(bool perFrame);
			void CreateFences();
			ID3D12CommandQueue* GetCommandQueue();
			ID3D12GraphicsCommandList* GetGraphicsCommandList();
			FrameResource m_frameResources[RENDER_FRAME_COUNT];
			SharedWindowPtr m_outputWindow;
#ifndef NDEBUG
			ComPtr<ID3D12Debug> m_d3d12Debug;
			ComPtr<IDXGIDebug> m_dxgiDebug;
			void EnableDebugLayer();
			void InitDXGIDebug();
#endif
		};
	}
}