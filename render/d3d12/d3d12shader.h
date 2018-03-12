#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <vector>
#include <unordered_map>
#include "d3d12shadermanager.h"
#include "d3d12descriptorheapmanager.h"
#include "ishader.h"

namespace LightningGE
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		enum class D3D12RootBoundResourceType
		{
			DescriptorTable,
			ConstantBufferView,
			ShaderResourceView,
			UnorderedAccessView,
			Constant
		};

		struct D3D12Constant32BitValue
		{
			const void* p32BitValues;
			UINT num32BitValues;
			UINT dest32BitValueOffset;
		};

		struct D3D12RootBoundResource
		{
			D3D12RootBoundResourceType type;
			ComPtr<ID3D12DescriptorHeap> descriptorTableHeap;
			union
			{
				D3D12_GPU_DESCRIPTOR_HANDLE descriptorTableHandle;
				D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress;
				D3D12Constant32BitValue constant32BitValue;
			};
		};

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
			void SetArgument(const ShaderArgument& argument) override;
			void Compile()override;
			void* GetByteCodeBuffer()const;
			SIZE_T GetByteCodeBufferSize()const;
			const std::vector<D3D12_ROOT_PARAMETER>& GetRootParameters()const;
			std::size_t GetRootParameterCount()const;
			const std::vector<D3D12RootBoundResource>& GetRootBoundResources()const;
		private:
			//use to commit constant to shader
			struct ConstantUploadContext
			{
				//upload heap use to update constant value
				ComPtr<ID3D12Resource> resource[RENDER_FRAME_COUNT];
				//handle to resource
				D3D12_CPU_DESCRIPTOR_HANDLE handle[RENDER_FRAME_COUNT];
				//constant buffer name in shader
				char* bufferName;
				//specified register index in shader
				UINT registerIndex;
			};
			struct ArgumentBinding
			{
				UINT bufferIndex;
				UINT offsetInBuffer;
			};
			void CompileImpl(IMemoryAllocator* memoryAllocator);
			D3D12_SHADER_VISIBILITY GetParameterVisibility()const;
			ComPtr<ID3D10Blob> m_byteCode;
			ComPtr<ID3D12ShaderReflection> m_shaderReflect;
			D3D12_SHADER_DESC m_desc;
			DescriptorHeap m_commitHeapInfo;
			std::vector<ConstantUploadContext> m_uploadContexts;
			std::unordered_map<std::string, ArgumentBinding> m_argumentBindings;
			std::vector<D3D12_ROOT_PARAMETER> m_rootParameters;
			std::unordered_map<std::uint8_t, std::vector<D3D12RootBoundResource>> m_rootBoundResources;
			D3D12_DESCRIPTOR_RANGE *m_descriptorRanges;
		};
	}
}