#include <d3dx12.h>
#include "D3D12Texture.h"
#include "Logger.h"

namespace Lightning
{
	namespace Render
	{
		D3D12Texture::D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device)
		{
			mResource = device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr);
			if (!mResource)
			{
				LOG_ERROR("Failed to create texture.");
				return;
			}
			UINT64 uploadBufferSize{ 0 };
			device->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
			mIntermediateResource = device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr);
			if (!mIntermediateResource)
			{
				LOG_ERROR("Failed to create intermediate texture!");
			}
		}

		D3D12Texture::D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, float depth, std::uint8_t stencil)
		{
			D3D12_CLEAR_VALUE clearValue;
			clearValue.Format = desc.Format;
			clearValue.DepthStencil.Depth = depth;
			clearValue.DepthStencil.Stencil = stencil;
			mResource = device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				&clearValue);

			if (!mResource)
			{
				LOG_ERROR("Failed to create texture.");
				return;
			}

			UINT64 uploadBufferSize{ 0 };
			device->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
			mIntermediateResource = device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr);
			if (!mIntermediateResource)
			{
				LOG_ERROR("Failed to create intermediate texture!");
			}
		}

		void D3D12Texture::Commit()
		{
			if (mCommitted)
				return;
		}
	}
}