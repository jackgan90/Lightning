#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "ishader.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class D3D12Shader : public Shader
		{
		public:
			D3D12Shader(ShaderType type);
			~D3D12Shader()override;
			std::string GetEntryPoint()const override; 
			ShaderType GetType()const override;
			const ShaderDefine GetMacros()const override;
			bool Compile(const Foundation::FilePtr& file, const ShaderDefine& define)override;
			const std::string GetCompileErrorLog()const override;
			std::string GetName()const override;
#ifdef DEBUG
			std::string GetSource()const override;
#endif
			void GetShaderModelVersion(int& major, int& minor)override;
			void* GetByteCodeBuffer()const;
			SIZE_T GetByteCodeBufferSize()const;
		private:
			void GetShaderModelString(char* buf)const;
			ShaderType m_type;
			std::string m_name;
#ifdef DEBUG
			std::string m_source;
#endif
			std::string m_compileError;
			int m_smMajorVersion;
			int m_smMinorVersion;
			ComPtr<ID3D10Blob> m_byteCode;
		};
	}
}