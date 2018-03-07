#include "d3d12rendertargetmanager.h"
#include "d3d12rendertarget.h"
#include "d3d12swapchain.h"

namespace LightningGE
{
	namespace Render
	{
		D3D12RenderTargetManager::D3D12RenderTargetManager() :m_currentID(1)
		{

		}

		D3D12RenderTargetManager::~D3D12RenderTargetManager()
		{
		}


		SharedRenderTargetPtr D3D12RenderTargetManager::CreateRenderTarget()
		{
			return SharedRenderTargetPtr();
		}

		SharedRenderTargetPtr D3D12RenderTargetManager::CreateSwapChainRenderTarget(const ComPtr<ID3D12Resource>& swapChainRT, 
			const D3D12_CPU_DESCRIPTOR_HANDLE& handle, const D3D12SwapChain* pSwapChain)
		{
			RenderTargetID rtID = m_currentID;
			D3D12RenderTarget *pRenderTarget = new D3D12RenderTarget(swapChainRT, true, rtID);
			pRenderTarget->m_cpuHandle = handle;
			pRenderTarget->m_sampleCount = pSwapChain->GetSampleCount();
			pRenderTarget->m_sampleQuality = pSwapChain->GetSampleQuality();
			pRenderTarget->m_format = pSwapChain->GetRenderFormat();
			m_renderTargets[rtID] = SharedRenderTargetPtr(pRenderTarget);
			m_currentID++;
			return m_renderTargets[rtID];
		}
	}
}