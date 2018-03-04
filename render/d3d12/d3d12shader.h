#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <vector>
#include "d3d12shadermanager.h"
#include "d3d12descriptorheapmanager.h"
#include "ishader.h"

namespace LightningGE
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
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
			std::vector<D3D12_ROOT_PARAMETER> GetRootParameters()const;
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
			const HeapAllocationInfo *m_commitHeapInfo;
			std::vector<ConstantUploadContext> m_uploadContexts;
			std::unordered_map<std::string, ArgumentBinding> m_argumentBindings;
			CD3DX12_ROOT_PARAMETER m_cbvParameter;
			std::vector<D3D12_ROOT_PARAMETER> m_rootParameters;
			D3D12_DESCRIPTOR_RANGE *m_descriptorRanges;
		};
	}
}