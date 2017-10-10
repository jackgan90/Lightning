#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#ifdef DEBUG
#include <dxgidebug.h>
#endif
#include "iwindow.h"
#include "renderer.h"
#include "filesystem.h"
#include "d3d12descriptorheapmanager.h"

#ifdef DEBUG
#define REPORT_LIVE_OBJECTS if(m_dxgiDebug) {m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);}
#else
#define REPORT_LIVE_OBJECTS
#endif

namespace LightningGE
{
	namespace Renderer
	{
		using WindowSystem::WindowPtr;
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12Renderer : public Renderer
		{
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			D3D12Renderer(const WindowPtr& pContext, const FileSystemPtr& fs);
			~D3D12Renderer()override;
			DevicePtr GetDevice()override;
			SwapChainPtr GetSwapChain()override;
			void SetClearColor(const ColorF& color)override;
			void ApplyPipelineStateObject(const PipelineStateObjectPtr& pso)override;
			std::shared_ptr<D3D12DescriptorHeapManager> GetDescriptorHeapManager()const noexcept
			{
				return m_descriptorMgr;
			}
		protected:
			void BeginRender();
			void DoRender();
			void EndRender();
		private:
			void WaitForPreviousFrame();
			void InitDevice(ComPtr<IDXGIFactory4> dxgiFactory);
			void InitSwapChain(ComPtr<IDXGIFactory4> dxgiFactory, const WindowPtr& pWindow);
			void CreateFences();
			FileSystemPtr m_fs;
			DevicePtr m_device;
			SwapChainPtr m_swapChain;
			std::vector<ComPtr<ID3D12Fence>> m_fences;
			std::vector<UINT64> m_fenceValues;
			std::shared_ptr<D3D12DescriptorHeapManager> m_descriptorMgr;
			HANDLE m_fenceEvent;
			PipelineStateObjectPtr m_pso;
			UINT m_currentBackBufferIndex;
			ColorF m_clearColor;
#ifdef DEBUG
			ComPtr<ID3D12Debug> m_d3d12Debug;
			ComPtr<IDXGIDebug> m_dxgiDebug;
			void EnableDebugLayer();
			void InitDXGIDebug();
#endif
		};
	}
}