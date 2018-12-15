#include <d3dx12.h>
#include "D3D12Texture.h"
#include "Logger.h"
#include "D3D12Renderer.h"

namespace Lightning
{
	namespace Render
	{
		using Loading::ISerializeBuffer;
		D3D12Texture::D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, ISerializeBuffer* buffer)
			:mDesc(mDesc), mBuffer(buffer), mCommitted(false)
		{
			if (mBuffer)
				mBuffer->AddRef();
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

		D3D12Texture::D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, ISerializeBuffer* buffer, float depth, std::uint8_t stencil)
			:mDesc(desc), mBuffer(buffer), mCommitted(false)
		{
			if (mBuffer)
				mBuffer->AddRef();
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

		D3D12Texture::~D3D12Texture()
		{
			if (mBuffer)
				mBuffer->Release();
		}

		void D3D12Texture::Commit()
		{
			if (mCommitted)
				return;
			if (!mBuffer)
				return;
			D3D12_SUBRESOURCE_DATA subresourceData{};
			subresourceData.pData = mBuffer->GetBuffer();
			subresourceData.RowPitch = GetBytesPerRow(mDesc.Format, std::uint32_t(mDesc.Width));
			subresourceData.SlicePitch = subresourceData.RowPitch * mDesc.Height;
			
			auto commandList = static_cast<D3D12Renderer*>(Renderer::Instance())->GetGraphicsCommandList();
			::UpdateSubresources(commandList, *mResource, *mIntermediateResource, 0, 0, 1, &subresourceData);
			mBuffer->Release();
			mBuffer = nullptr;
			mCommitted = true;
		}

		std::uint16_t D3D12Texture::GetBitsPerPixel(DXGI_FORMAT format)
		{
			switch (format)
			{
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				return 128;
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
				return 64;
			case DXGI_FORMAT_R16G16B16A16_UNORM:
				return 64;
			case DXGI_FORMAT_R8G8B8A8_UNORM:
				return 32;
			case DXGI_FORMAT_B8G8R8A8_UNORM:
				return 32;
			case DXGI_FORMAT_B8G8R8X8_UNORM:
				return 32;
			case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
				return 32;
			case DXGI_FORMAT_R10G10B10A2_UNORM:
				return 32;
			case DXGI_FORMAT_B5G5R5A1_UNORM:
				return 16;
			case DXGI_FORMAT_B5G6R5_UNORM:
				return 16;
			case DXGI_FORMAT_R32_FLOAT:
				return 32;
			case DXGI_FORMAT_R16_FLOAT:
				return 16;
			case DXGI_FORMAT_R16_UNORM:
				return 16;
			case DXGI_FORMAT_R8_UNORM:
				return 8;
			case DXGI_FORMAT_A8_UNORM:
				return 8;
			default:
				return 0;
			}
		}

		std::uint32_t D3D12Texture::GetBytesPerRow(DXGI_FORMAT format, std::uint32_t width)
		{
			return width * GetBitsPerPixel(format) / 8;
		}
	}
}