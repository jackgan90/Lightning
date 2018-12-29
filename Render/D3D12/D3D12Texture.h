#pragma once
#include <mutex>
#include "ITexture.h"
#include "D3D12StatefulResource.h"
#include "ISerializeBuffer.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Render
	{
		class D3D12Device;
		class D3D12Texture : public ITexture
		{
		public:
			//Create normal texture
			D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, Loading::ISerializeBuffer* buffer);
			//Create for depth or stencil buffer
			D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, const float depth, const std::uint8_t stencil);
			D3D12Texture(D3D12Device* device, const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState);
			~D3D12Texture()override;
			void INTERFACECALL Commit()override;
			std::uint16_t INTERFACECALL GetMultiSampleCount()const override;
			std::uint16_t INTERFACECALL GetMultiSampleQuality()const override;
			RenderFormat INTERFACECALL GetRenderFormat()const override;
			std::size_t INTERFACECALL GetWidth()const override;
			std::size_t INTERFACECALL GetHeight()const override;
			D3D12StatefulResourcePtr GetResource()const { return mResource; }
			static std::uint16_t GetBitsPerPixel(DXGI_FORMAT format);
			static std::uint32_t GetBytesPerRow(DXGI_FORMAT format, std::uint32_t width);
		private:
			void InitIntermediateResource();
			bool mCommitted;
			D3D12_RESOURCE_DESC mDesc;
			D3D12StatefulResourcePtr mResource;
			D3D12StatefulResourcePtr mIntermediateResource;
			Loading::ISerializeBuffer* mBuffer;
			D3D12Device* mDevice;
			std::mutex mCommitMutex;
			REF_OBJECT_OVERRIDE(D3D12Texture)
		};
	}
}