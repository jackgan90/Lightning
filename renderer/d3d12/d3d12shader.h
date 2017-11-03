#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "d3d12shadermanager.h"
#include "ishader.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class D3D12Shader : public Shader
		{
		public:
			friend class D3D12Device;
			friend class D3D12ShaderManager;
			D3D12Shader(ShaderType type, const std::string& name, const char* const shaderSource, const ComPtr<ID3D10Blob> byteCode,
				int smMajor, int smMinor, const std::string& entry);
			~D3D12Shader()override;
			std::string GetEntryPoint()const override; 
			ShaderType GetType()const override;
			const ShaderDefine GetMacros()const override;
			//bool Compile(const Foundation::SharedFilePtr& file, const ShaderDefine& define)override;
			//const std::string GetCompileErrorLog()const override;
			std::string GetName()const override;
#ifndef NDEBUG
			const char* const GetSource()const override;
#endif
			void GetShaderModelVersion(int& major, int& minor)override;
			void* GetByteCodeBuffer()const;
			SIZE_T GetByteCodeBufferSize()const;
		private:
			ShaderType m_type;
			std::string m_name;
			std::string m_entry;
#ifndef NDEBUG
			const char* const m_source;
#endif
			int m_smMajorVersion;
			int m_smMinorVersion;
			ComPtr<ID3D10Blob> m_byteCode;
		};
	}
}