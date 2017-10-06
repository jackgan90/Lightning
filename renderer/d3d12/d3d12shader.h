#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "ishader.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class D3D12Shader : public IShader
		{
		public:
			D3D12Shader(ShaderType type);
			std::string GetEntryPoint()const override; 
			void SetEntryPoint(const std::string& entryPoint)override;
			ShaderType GetType()const override;
			void DefineMacro(const ShaderDefine& define)override;
			const ShaderDefine GetMacro()const override;
			bool Compile(const Foundation::FilePtr& file, const ShaderDefine& define)override;
			const std::string GetCompileErrorLog()const override;
			std::string GetName()const override;
#ifdef DEBUG
			std::string GetSource()const override;
#endif
			void GetShaderModelVersion(int& major, int& minor)override;
			void ReleaseRenderResources()override;
		private:
			void GetShaderModelString(char* buf)const;
			ShaderType m_type;
			std::string m_entryPoint;
			ShaderDefine m_macros;
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