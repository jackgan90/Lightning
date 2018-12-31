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
			D3D12Device* GetDevice()const { return mDevice; }
			D3D12StatefulResourcePtr GetResource()const { return mResource; }
			float GetDepthClearValue()const { return mClearValue.DepthStencil.Depth; }
			std::uint8_t GetStencilClearValue()const { return mClearValue.DepthStencil.Stencil; }
			//Generally speaking,a texture's size is fixed upon creation.Users of it should not resize it.
			//But for the back buffer depth stencil texture,it indeed is resizable.This method only intends to do 
			//depth stencil resizing.
			//You should never call it on other circumstances.;
			void Resize(std::size_t width, std::size_t height);
			static std::uint16_t GetBitsPerPixel(DXGI_FORMAT format);
			static std::uint32_t GetBytesPerRow(DXGI_FORMAT format, std::uint32_t width);
		private:
			void InitIntermediateResource();
			bool mCommitted;
			D3D12_CLEAR_VALUE mClearValue;
			D3D12StatefulResourcePtr mResource;
			D3D12StatefulResourcePtr mIntermediateResource;
			Loading::ISerializeBuffer* mBuffer;
			D3D12Device* mDevice;
			std::mutex mCommitMutex;
			REF_OBJECT_OVERRIDE(D3D12Texture)
		};
	}
}