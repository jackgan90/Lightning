#include "D3D12RenderTargetManager.h"
#include "D3D12RenderTarget.h"
#include "D3D12SwapChain.h"

namespace Lightning
{
	namespace Render
	{
		D3D12RenderTargetManager::D3D12RenderTargetManager() :mCurrentID(1)
		{

		}

		D3D12RenderTargetManager::~D3D12RenderTargetManager()
		{
		}


		IRenderTarget* D3D12RenderTargetManager::CreateRenderTarget()
		{
			return nullptr;
		}

		IRenderTarget* D3D12RenderTargetManager::CreateSwapChainRenderTarget(
			const D3D12StatefulResourcePtr& resource, D3D12SwapChain* pSwapChain)
		{
			RenderTargetID rtID = mCurrentID.fetch_add(1, std::memory_order_relaxed);
			auto ptr = NEW_REF_OBJ(D3D12RenderTarget, rtID, resource, pSwapChain);
			StoreRenderTarget(rtID, ptr);
			return ptr;
		}
	}
}