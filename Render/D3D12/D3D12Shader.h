#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "ThreadLocalSingleton.h"
#include "Container.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12Texture.h"
#include "Texture/Sampler.h"
#include "Shader.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::Container;
		enum class D3D12RootResourceType : std::uint8_t
		{
			ConstantBuffers,
			Textures,
			Samplers
		};

		struct D3D12Constant32BitValue
		{
			const void* p32BitValues;
			UINT num32BitValues;
			UINT dest32BitValueOffset;
		};

		struct D3D12RootBoundResource
		{
			D3D12RootResourceType type;
			union
			{
				D3D12ConstantBuffer* buffers;
				D3D12Texture** textures;
				SamplerState* samplerStates;
			};
			std::size_t count;
		};

		//Thread unsafe
		class D3D12Shader : public Shader
		{
		public:
			D3D12Shader(D3D_SHADER_MODEL shaderModel, ShaderType type, const std::string& name, const char* const shaderSource);
			INTERFACECALL ~D3D12Shader()override;
			const IShaderMacros* INTERFACECALL GetMacros()const override;
			std::size_t INTERFACECALL GetParameterCount()const override;
			bool INTERFACECALL SetParameter(const IShaderParameter* parameter) override;
			void INTERFACECALL Compile()override;
			void INTERFACECALL GetUniformSemantics(RenderSemantics** semantics, std::uint16_t& semanticCount)override;
			void* GetByteCodeBuffer()const;
			SIZE_T GetByteCodeBufferSize()const;
			const Container::Vector<D3D12_ROOT_PARAMETER>& GetRootParameters()const;
			std::size_t GetRootParameterCount()const;
			const D3D12RootBoundResource* GetRootBoundResources();
			UINT GetConstantBufferCount();
			UINT GetConstantBufferSize();
		private:
			void GetShaderModelString(char* buf);
			void CompileImpl();
			D3D12_SHADER_VISIBILITY GetParameterVisibility()const;
			void UpdateRootBoundResources();
			void InitConstantBufferRootParameter(ID3D12ShaderReflection*, 
				const Container::UnorderedMap<std::string, D3D12_SHADER_INPUT_BIND_DESC>&);
			void InitTextureRootParameter(const Container::UnorderedMap<std::string, D3D12_SHADER_INPUT_BIND_DESC>&);
			void InitSamplerStateParameter(const Container::UnorderedMap<std::string, D3D12_SHADER_INPUT_BIND_DESC>&);
		private:
			struct ParameterInfo
			{
				UINT bufferIndex;
				UINT offsetInBuffer;
			};
			struct ConstantBufferInfo
			{
				UINT offset;
				UINT size;
			};
			class ShaderResourceProxy
			{
			public:
				ShaderResourceProxy();
				~ShaderResourceProxy();
				std::uint8_t* GetConstantBuffer(std::size_t size);
				void Init(std::size_t totalCount, std::size_t constantBufferCount, std::size_t textureCount, std::size_t samplerStateCount);
				//Must be called at the start of resource update
				void BeginUpdateResource(D3D12RootResourceType resourceType);
				//Must be called on resource update end.It's a counterpart of BeginUpdateResource
				void EndUpdateResource();
				void AddConstantBuffer(const D3D12ConstantBuffer& constantBuffer);
				const D3D12RootBoundResource* GetRootBoundResources();
			private:
				D3D12RootBoundResource* mRootBoundResources[RENDER_FRAME_COUNT];
				std::uint8_t *mConstantBuffer;
				std::size_t mBufferSize;
				std::size_t mRootResourceCount;
				std::size_t mCurrentResourceIndex;
				std::size_t mCurrentBufferIndex;
				bool mInit;
			};
		private:
			ComPtr<ID3D10Blob> mByteCode;
			D3D12_SHADER_DESC mDesc;
			Container::UnorderedMap<std::string, ParameterInfo> mParameters;
			Container::Vector<D3D12_ROOT_PARAMETER> mRootParameters;
			//each offset corresponds to mIntermediateBuffer
			Container::UnorderedMap<std::size_t, ConstantBufferInfo> mConstantBufferInfo;
			Container::Vector<D3D12_DESCRIPTOR_RANGE> mDescriptorRanges;
			//buffer used to cache constant buffer value
			Foundation::ThreadLocalSingleton<ShaderResourceProxy> mResourceProxy;
			Container::Vector<RenderSemantics> mUniformSemantics;
			D3D_SHADER_MODEL mShaderModel;
			UINT mTotalConstantBufferSize;
			UINT mTextureParameterCount;
			UINT mSamplerStateParamCount;
		};
	}
}