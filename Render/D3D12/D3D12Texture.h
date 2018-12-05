#pragma once
#include "ITexture.h"
#include "D3D12StatefulResource.h"
#include "D3D12Device.h"

namespace Lightning
{
	namespace Render
	{
		class D3D12Texture : public ITexture
		{
		public:
			//Create normal texture
			D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device);
			//Create for depth or stencil buffer
			D3D12Texture(const D3D12_RESOURCE_DESC& desc, D3D12Device* device, float depth, std::uint8_t stencil);
			void Commit()override;
		private:
			bool mCommitted;
			D3D12StatefulResourcePtr mResource;
			D3D12StatefulResourcePtr mIntermediateResource;
		};
	}
}