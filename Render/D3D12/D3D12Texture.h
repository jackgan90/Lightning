#pragma once
#include "ITexture.h"
#include "D3D12StatefulResource.h"
#include "D3D12Device.h"
#include "ISerializeBuffer.h"

namespace Lightning
{
	namespace Render
	{
		class D3D12Texture : public ITexture
		{
		public:
			//Create normal texture
			D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, Loading::ISerializeBuffer* buffer);
			//Create for depth or stencil buffer
			D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, Loading::ISerializeBuffer* buffer, float depth, std::uint8_t stencil);
			~D3D12Texture()override;
			void Commit()override;
			static std::uint16_t GetBitsPerPixel(DXGI_FORMAT format);
			static std::uint32_t GetBytesPerRow(DXGI_FORMAT format, std::uint32_t width);
		private:
			bool mCommitted;
			D3D12_RESOURCE_DESC mDesc;
			D3D12StatefulResourcePtr mResource;
			D3D12StatefulResourcePtr mIntermediateResource;
			Loading::ISerializeBuffer* mBuffer;
		};
	}
}