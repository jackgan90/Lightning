#include <d3dx12.h>
#include "Logger.h"
#include "D3D12Device.h"
#include "D3D12Texture.h"
#include "D3D12Renderer.h"

namespace Lightning
{
	namespace Render
	{
		using Loading::ISerializeBuffer;
		D3D12Texture::D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, ISerializeBuffer* buffer)
			:mCommitted(false), mDesc(desc), mBuffer(buffer), mDevice(device)
		{
			if (mBuffer)
				mBuffer->AddRef();
			mResource = mDevice->CreateCommittedResource(
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
			InitIntermediateResource();
		}

		D3D12Texture::D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, const float depth, const std::uint8_t stencil)
			:mCommitted(false), mDesc(desc), mBuffer(nullptr), mDevice(device)
		{
			D3D12_CLEAR_VALUE clearValue;
			clearValue.Format = desc.Format;
			clearValue.DepthStencil.Depth = depth;
			clearValue.DepthStencil.Stencil = stencil;
			mResource = mDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue);

			if (!mResource)
			{
				LOG_ERROR("Failed to depth texture.");
				return;
			}
		}
		
		D3D12Texture::D3D12Texture(D3D12Device* device, const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState)
			:mCommitted(true), mBuffer(nullptr), mDevice(device)
		{
			mResource = std::make_shared<D3D12StatefulResource>(resource, initialState);
			mDesc = resource->GetDesc();
		}

		D3D12Texture::~D3D12Texture()
		{
			mResource.reset();
			mIntermediateResource.reset();
			if (mBuffer)
			{
				mBuffer->Release();
			}
		}

		void D3D12Texture::InitIntermediateResource()
		{
			UINT64 uploadBufferSize{ 0 };
			mDevice->GetCopyableFootprints(&mDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
			mIntermediateResource = mDevice->CreateCommittedResource(
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
			if (!mBuffer)
				return;
			{
				std::lock_guard<std::mutex> lock(mCommitMutex);
				if (!mCommitted && mBuffer) //Do double check,because there may be more than one thread enter lock section
				{
					D3D12_SUBRESOURCE_DATA subresourceData{};
					subresourceData.pData = mBuffer->GetBuffer();
					subresourceData.RowPitch = GetBytesPerRow(mDesc.Format, std::uint32_t(mDesc.Width));
					subresourceData.SlicePitch = subresourceData.RowPitch * mDesc.Height;
					
					auto D3DRenderer = dynamic_cast<D3D12Renderer*>(Renderer::Instance());
					assert(D3DRenderer != nullptr && "A D3D12Renderer is required.");
					auto commandList = D3DRenderer->GetGraphicsCommandList();
					::UpdateSubresources(commandList, *mResource, *mIntermediateResource, 0, 0, 1, &subresourceData);
					mBuffer->Release();
					mBuffer = nullptr;
					mCommitted = true;
				}
			}
		}

		std::uint16_t D3D12Texture::GetMultiSampleCount()const
		{
			return static_cast<std::uint16_t>(mDesc.SampleDesc.Count);
		}

		std::uint16_t D3D12Texture::GetMultiSampleQuality()const
		{
			return static_cast<std::uint16_t>(mDesc.SampleDesc.Quality);
		}

		RenderFormat D3D12Texture::GetRenderFormat()const
		{
			return D3D12TypeMapper::MapRenderFormat(mDesc.Format);
		}

		std::size_t D3D12Texture::GetWidth()const
		{
			return mDesc.Width;
		}

		std::size_t D3D12Texture::GetHeight()const
		{
			return mDesc.Height;
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