#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "ThreadLocalObject.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12Texture.h"
#include "Texture/Sampler.h"
#include "Shader.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
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
			const std::vector<D3D12_ROOT_PARAMETER>& GetRootParameters()const;
			std::size_t GetRootParameterCount()const;
			const D3D12RootBoundResource* GetRootBoundResources();
			UINT GetConstantBufferCount()const;
			UINT GetTextureCount()const;
			UINT GetSamplerStateCount()const;
			UINT GetConstantBufferSize();
		private:
			void GetShaderModelString(char* buf);
			void CompileImpl();
			D3D12_SHADER_VISIBILITY GetParameterVisibility()const;
			void UpdateRootBoundResources();
			void InitConstantBufferRootParameter(ID3D12ShaderReflection*, 
				const std::unordered_map<std::string, D3D12_SHADER_INPUT_BIND_DESC>&);
			void InitTextureRootParameter(const std::unordered_map<std::string, D3D12_SHADER_INPUT_BIND_DESC>&);
			void InitSamplerStateParameter(const std::unordered_map<std::string, D3D12_SHADER_INPUT_BIND_DESC>&);
		private:
			struct ParameterInfo
			{
				UINT index;
				UINT offset;
			};
			struct ConstantBufferInfo
			{
				UINT offset;
				UINT size;
			};
			using ConstantBufferInfos = std::unordered_map<std::size_t, ConstantBufferInfo>;
			class ShaderResourceProxy
			{
			public:
				ShaderResourceProxy();
				~ShaderResourceProxy();
				std::uint8_t* GetConstantBuffer();
				void Init(ConstantBufferInfos* constantBufferInfos, std::size_t textureCount
					, std::size_t samplerStateCount, std::size_t constantBufferSize);
				//Must be called at the start of resource update
				const D3D12RootBoundResource* GetRootBoundResources();
				void CommitResources();
				void SetConstantBuffer(std::size_t offset, const void* buffer, std::size_t size);
				void SetTexture(UINT index, D3D12Texture* texture);
				void SetSamplerState(UINT index, const SamplerState& samplerState);
			private:
				D3D12RootBoundResource* mRootBoundResources[RENDER_FRAME_COUNT];
				std::uint8_t *mConstantBuffer;
				ConstantBufferInfos* mConstantBufferInfos;
				std::size_t mResourceCount;
				int mConstantBufferResourceIndex;
				int mTextureResourceIndex;
				int mSamplerResourceIndex;
				bool mInit;
			};
		private:
			void InitResourceProxy();
		private:
			ComPtr<ID3D10Blob> mByteCode;
			D3D12_SHADER_DESC mDesc;
			std::unordered_map<std::string, ParameterInfo> mParameters;
			std::vector<D3D12_ROOT_PARAMETER> mRootParameters;
			//each offset corresponds to mIntermediateBuffer
			ConstantBufferInfos mConstantBufferInfo;
			std::vector<D3D12_DESCRIPTOR_RANGE> mDescriptorRanges;
			//buffer used to cache constant buffer value
			Foundation::ThreadLocalObject<ShaderResourceProxy> mResourceProxy;
			std::vector<RenderSemantics> mUniformSemantics;
			D3D_SHADER_MODEL mShaderModel;
			UINT mTotalConstantBufferSize;
			UINT mTextureParameterCount;
			UINT mSamplerStateParamCount;
		};
	}
}