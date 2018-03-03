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
			D3D12Shader(ShaderType type, const std::string& name, const std::string& entry, const char* const shaderSource);
			~D3D12Shader()override;
			const ShaderDefine GetMacros()const override;
			//bool Compile(const Foundation::SharedFilePtr& file, const ShaderDefine& define)override;
			//const std::string GetCompileErrorLog()const override;
			std::size_t GetArgumentCount()const override;
			void SetArgument(const ShaderArgument& argument) override;
			void Compile()override;
			void* GetByteCodeBuffer()const;
			SIZE_T GetByteCodeBufferSize()const;
		private:
			void CompileImpl(IMemoryAllocator* memoryAllocator);
			ComPtr<ID3D10Blob> m_byteCode;
			ComPtr<ID3D12ShaderReflection> m_shaderReflect;
			D3D12_SHADER_DESC m_desc;
			const HeapAllocationInfo *m_commitHeapInfo;
			std::vector<ComPtr<ID3D12Resource>> m_constantBuffers;
		};
	}
}