#include "d3d12rendertargetmanager.h"
#include "d3d12rendertarget.h"
#include "d3d12swapchain.h"

namespace Lightning
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

		SharedRenderTargetPtr D3D12RenderTargetManager::CreateSwapChainRenderTarget(
			const ComPtr<ID3D12Resource>& resource, ID3D12Device* pDevice, D3D12SwapChain* pSwapChain)
		{
			RenderTargetID rtID = m_currentID;
			m_renderTargets[rtID] = SharedRenderTargetPtr(new D3D12RenderTarget(rtID, resource, pDevice, pSwapChain));
			m_currentID++;
			return m_renderTargets[rtID];
		}
	}
}