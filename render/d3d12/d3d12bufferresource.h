#pragma once
#include <d3d12.h>
#include <wrl\client.h>
#include <cstdint>
#include "rendererexportdef.h"

namespace LightningGE
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDER_API D3D12BufferResource
		{
		public:
			D3D12BufferResource(ID3D12Device* pDevice, uint32_t bufferSize);
			ID3D12Resource* GetUploadHeap() { return m_uploadHeap.Get(); }
			ID3D12Resource* GetDefaultHeap() { return m_defaultHeap.Get(); }
			D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() { return m_defaultHeap->GetGPUVirtualAddress(); }
			uint32_t GetSize()const { return m_size; }
			void UpdateAccessState(D3D12_RESOURCE_STATES state) { m_accessState = state; }
			D3D12_RESOURCE_STATES GetAccessState()const { return m_accessState; }
		private:
			ComPtr<ID3D12Resource> m_uploadHeap;
			ComPtr<ID3D12Resource> m_defaultHeap;
			std::uint32_t m_size;
			D3D12_RESOURCE_STATES m_accessState;
		};
	}
}