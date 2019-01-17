#include <d3dx12.h>
#include <algorithm>
#include "Logger.h"
#include "D3D12Device.h"
#include "D3D12Texture.h"
#include "D3D12Renderer.h"
#undef max

namespace Lightning
{
	namespace Render
	{
		using Loading::ISerializeBuffer;
		D3D12Texture::D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, ISerializeBuffer* buffer)
			:mCommitted(false), mBuffer(buffer), mDevice(device)
		{
			mClearValue.Format = DXGI_FORMAT_UNKNOWN;
			mClearValue.DepthStencil.Depth = 1.0f;
			mClearValue.DepthStencil.Stencil = 0;
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
			:mCommitted(false), mBuffer(nullptr), mDevice(device)
		{
			mClearValue.Format = desc.Format;
			mClearValue.DepthStencil.Depth = depth;
			mClearValue.DepthStencil.Stencil = stencil;
			mResource = mDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&mClearValue);

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
			mDevice->GetCopyableFootprints(&mResource->GetDesc(), 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
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

		TextureDimension D3D12Texture::GetDimension()const
		{
			const auto& desc = mResource->GetDesc();
			if (desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D)
			{
				if (desc.DepthOrArraySize > 1)
				{
					return TEXTURE_DIMENSION_1D_ARRAY;
				}
				else
				{
					return TEXTURE_DIMENSION_1D;
				}
			}

			if (desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
			{
				if (desc.DepthOrArraySize > 1)
				{
					return TEXTURE_DIMENSION_2D_ARRAY;
				}
				else
				{
					return TEXTURE_DIMENSION_2D;
				}
			}
			return TEXTURE_DIMENSION_3D;
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
					const auto& desc = mResource->GetDesc();
					D3D12_SUBRESOURCE_DATA subresourceData{};
					subresourceData.pData = mBuffer->GetBuffer();
					subresourceData.RowPitch = GetBytesPerRow(desc.Format, std::uint32_t(desc.Width));
					subresourceData.SlicePitch = subresourceData.RowPitch * desc.Height;
					
					auto D3DRenderer = dynamic_cast<D3D12Renderer*>(Renderer::Instance());
					assert(D3DRenderer != nullptr && "A D3D12Renderer is required.");
					auto commandList = D3DRenderer->GetGraphicsCommandList();
					::UpdateSubresources(commandList, mResource->GetResource(), mIntermediateResource->GetResource(), 0, 0, 1, &subresourceData);
					mResource->TransitTo(commandList, D3D12_RESOURCE_STATE_GENERIC_READ);
					mBuffer->Release();
					mBuffer = nullptr;
					mCommitted = true;
				}
			}
		}

		std::uint16_t D3D12Texture::GetMultiSampleCount()const
		{
			const auto& desc = mResource->GetDesc();
			return static_cast<std::uint16_t>(desc.SampleDesc.Count);
		}

		std::uint16_t D3D12Texture::GetMultiSampleQuality()const
		{
			const auto& desc = mResource->GetDesc();
			return static_cast<std::uint16_t>(desc.SampleDesc.Quality);
		}

		RenderFormat D3D12Texture::GetRenderFormat()const
		{
			const auto& desc = mResource->GetDesc();
			return D3D12TypeMapper::MapRenderFormat(desc.Format);
		}

		std::size_t D3D12Texture::GetWidth()const
		{
			const auto& desc = mResource->GetDesc();
			return desc.Width;
		}

		std::size_t D3D12Texture::GetHeight()const
		{
			const auto& desc = mResource->GetDesc();
			return desc.Height;
		}

		std::size_t D3D12Texture::GetDepth()const
		{
			const auto& desc = mResource->GetDesc();
			return desc.DepthOrArraySize;
		}

		std::size_t D3D12Texture::GetMipmapLevels()const
		{
			const auto& desc = mResource->GetDesc();
			return desc.MipLevels;
		}

		void D3D12Texture::Resize(std::size_t width, std::size_t height)
		{
			assert(mBuffer == nullptr && "Only textures with null serialized buffer is allowed to be resized.");
			assert(mIntermediateResource.get() == nullptr && "Only textures without uploading ability is allowed to be resized.");
			auto resourceState = mResource->GetGlobalState();
			//Don't create new D3D12StatefulResourcePtr object,just reuse the existing one.
			//Because there may still be reference to the old one.
			//Create new one and replace old one with newly created resource
			D3D12_RESOURCE_DESC desc(mResource->GetDesc());
			desc.Width = std::max(std::size_t(1), width);
			desc.Height = UINT(std::max(std::size_t(1), height));
			auto newResource = mDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&desc,
				resourceState,
				&mClearValue);
			mResource->Reset(newResource);
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