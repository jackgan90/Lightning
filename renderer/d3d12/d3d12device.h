#pragma once
#include "idevice.h"
#include <d3d12.h>
#include <wrl\client.h>

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12Device : public IDevice
		{
		public:
			friend class D3D12RenderContext;
			D3D12Device(ComPtr<ID3D12Device> pDevice);
			~D3D12Device()override;
			RenderTargetPtr CreateRenderTarget() override;
		private:
			ComPtr<ID3D12Device> m_device;
			ComPtr<ID3D12CommandQueue> m_commandQueue;
		};
	}
}