#include "d3d12rendertargetmanager.h"
#include "d3d12rendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12RenderTargetManager::D3D12RenderTargetManager(DevicePtr pDevice, SwapChainPtr pSwapChain) :m_pDevice(pDevice), m_pSwapChain(pSwapChain)
			,m_currentID(1)
		{

		}

		RenderTargetPtr D3D12RenderTargetManager::CreateRenderTarget()
		{
			return RenderTargetPtr();
		}

		RenderTargetPtr D3D12RenderTargetManager::CreateSwapChainRenderTarget(ComPtr<ID3D12Resource> swapChainRT, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
		{
			RenderTargetID rtID = m_currentID;
			D3D12RenderTarget *pRenderTarget = new D3D12RenderTarget(swapChainRT, true, rtID);
			pRenderTarget->m_cpuHandle = handle;
			m_renderTargets[rtID] = RenderTargetPtr(pRenderTarget);
			m_currentID++;
			return m_renderTargets[rtID];
		}


		void D3D12RenderTargetManager::ReleaseRenderResources()
		{
			for (auto it : m_renderTargets)
				it.second->ReleaseRenderResources();
			m_renderTargets.clear();
		}
	}
}