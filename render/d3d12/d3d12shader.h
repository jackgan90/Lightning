#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "threadlocalsingleton.h"
#include "container.h"
#include "d3d12constantbuffermanager.h"
#include "d3d12descriptorheapmanager.h"
#include "ishader.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::container;
		enum class D3D12RootResourceType
		{
			ConstantBuffers,
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
			container::vector<D3D12ConstantBuffer> buffers;
		};

		//Thread unsafe
		class D3D12Shader : public Shader
		{
		public:
			friend class D3D12Device;
			friend class D3D12ShaderManager;
			D3D12Shader(ID3D12Device* device, ShaderType type, const std::string& name, const std::string& entry, const char* const shaderSource);
			~D3D12Shader()override;
			const ShaderDefine GetMacros()const override;
			//bool Compile(const Foundation::SharedFilePtr& file, const ShaderDefine& define)override;
			//const std::string GetCompileErrorLog()const override;
			std::size_t GetArgumentCount()const override;
			bool SetArgument(const ShaderArgument& argument) override;
			void Compile()override;
			void* GetByteCodeBuffer()const;
			SIZE_T GetByteCodeBufferSize()const;
			const container::vector<D3D12_ROOT_PARAMETER>& GetRootParameters()const;
			std::size_t GetRootParameterCount()const;
			const container::vector<D3D12RootBoundResource>& GetRootBoundResources();
			UINT GetConstantBufferCount();
			UINT GetConstantBufferSize();
		private:
			struct ArgumentInfo
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
				std::uint8_t* GetConstantBuffer(std::size_t size);
				container::vector<D3D12RootBoundResource>& GetRootBoundResources();
				~ShaderResourceProxy();
			private:
				container::vector<D3D12RootBoundResource> mRootBoundResources[RENDER_FRAME_COUNT];
				std::uint8_t *mConstantBuffer;
				std::size_t mBufferSize;
			};
			void CompileImpl();
			D3D12_SHADER_VISIBILITY GetParameterVisibility()const;
			void UpdateRootBoundResources();
			ComPtr<ID3D10Blob> mByteCode;
			D3D12_SHADER_DESC mDesc;
			container::unordered_map<std::string, ArgumentInfo> mArguments;
			container::vector<D3D12_ROOT_PARAMETER> mRootParameters;
			//each offset corresponds to mIntermediateBuffer
			container::unordered_map<std::size_t, ConstantBufferInfo> mConstantBufferInfo;
			D3D12_DESCRIPTOR_RANGE *mDescriptorRanges;
			UINT mTotalConstantBufferSize;
			//buffer used to cache constant buffer value
			Foundation::ThreadLocalSingleton<ShaderResourceProxy> mResourceProxy;
		};
	}
}