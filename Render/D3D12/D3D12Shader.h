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
			D3D12Shader(D3D_SHADER_MODEL shaderModel, ShaderType type, 
				const std::string& name, const std::string& shaderSource, const std::shared_ptr<IShaderMacros>& macros);
			~D3D12Shader()override;
			std::size_t GetParameterCount()const override;
			bool SetParameter(const Parameter& parameter) override;
			ParameterType GetParameterType(const std::string& name)const override;
			void Compile()override;
			void GetUniformSemantics(RenderSemantics** semantics, std::uint16_t& semanticCount)override;
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
			static ParameterType GetParameterType(const D3D12_SHADER_TYPE_DESC& desc);
		private:
			struct ParameterInfo
			{
				//For constant buffer,the index is constant buffer index, for texture and sampler,the index is texture/sampler index
				UINT index;
				//For constant buffer,the offset is Bytes from constant buffer start,for texture/sampler,this field is unused.
				UINT offset;
				ParameterType parameterType;
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
			std::shared_ptr<IShaderMacros> mMacros;
		};
	}
}