#include <d3dcompiler.h>
#include <sstream>
#include "d3d12shader.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::FileSize;
		using Foundation::FilePointerType;
		using Foundation::FileAnchor;
		D3D12Shader::D3D12Shader(ShaderType type):m_type(type), m_entryPoint("main")
			,m_name(""), m_compileError(""), m_smMajorVersion(5), m_smMinorVersion(0)
#ifdef DEBUG
			,m_source("")
#endif
		{

		}

		std::string D3D12Shader::GetEntryPoint()const
		{
			return m_entryPoint;
		}

		void D3D12Shader::SetEntryPoint(const std::string& entryPoint)
		{
			m_entryPoint = entryPoint;
		}

		ShaderType D3D12Shader::GetType()const
		{
			return m_type;
		}

		void D3D12Shader::DefineMacro(const ShaderDefine& define)
		{
			m_macros += define;
		}

		const ShaderDefine D3D12Shader::GetMacro()const
		{
			return m_macros;
		}

		bool D3D12Shader::Compile(const Foundation::FilePtr& file, const ShaderDefine& define)
		{
			m_compileError = "";
			if (!file)
			{
				m_compileError = "Shader file is missing!";
				return false;
			}
			//we assume the file is at its beginning
			FileSize size = file->GetSize();
			if (size <= 0)
			{
				std::stringstream ss;
				ss << "Shader file is empty!file:" << file->GetPath();
				m_compileError = ss.str();
				return false;
			}
			file->SetFilePointer(FilePointerType::Read, FileAnchor::Begin, 0);
			char* buffer = new char[size + 1];
			FileSize readSize = file->Read(buffer, size);
			if (readSize < size)
			{
				std::stringstream ss;
				ss << "Failed to read shader file.Shader name:" << file->GetPath() << ",file size:" << size << ",read size:" << readSize;
				m_compileError = ss.str();
				delete[] buffer;
				return false;
			}
			buffer[size] = 0;
			D3D_SHADER_MACRO* pMacros = nullptr;
			size_t macroCount = define.GetMacroCount();
			if (macroCount)
			{
				pMacros = new D3D_SHADER_MACRO[macroCount + 1];
				std::memset(&pMacros[macroCount], 0, sizeof(D3D_SHADER_MACRO));
				DefineMap defineMap = define.GetAllDefine();
				size_t idx = 0;
				for (auto it = defineMap.begin(); it != defineMap.end(); ++it,++idx)
				{
					const char* name = it->first.c_str();
					pMacros[idx].Name = new char[std::strlen(name) + 1];
					std::strcpy(const_cast<char*>(pMacros[idx].Name), name);
					const char* definition = it->second.c_str();
					pMacros[idx].Definition = new char[std::strlen(definition) + 1];
					std::strcpy(const_cast<char*>(pMacros[idx].Definition), definition);
				}
			}
			//TODO: resolve include
#ifdef DEBUG
			UINT flags1 = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			UINT flags1 = 0;
#endif
			//TODO : flags2 is used to compile effect file.Should implemente it later
			UINT flags2 = 0;
			ComPtr<ID3DBlob> errorLog;
			char shaderModel[32];
			GetShaderModelString(shaderModel);
			HRESULT hr = ::D3DCompile(buffer, size + 1, nullptr, pMacros, nullptr, m_entryPoint.c_str(),
				shaderModel, flags1, flags2, &m_byteCode, &errorLog);
			if (FAILED(hr))
			{
				delete[] buffer;
				std::stringstream ss;
				ss << "Compile shader " << file->GetPath() << " failed!";
				if (macroCount)
				{
					ss << "Defined macros:" << std::endl;
					for (int i = 0; i < macroCount; i++)
					{
						ss << pMacros[i].Name << ":" << pMacros[i].Definition << std::endl;
						delete[] pMacros[i].Name;
						delete[] pMacros[i].Definition;
					}
				}
				if (pMacros)
				{
					delete[] pMacros;
				}
				ss << "Detailed info:" << std::endl;
				size_t compileErrorBufferSize = errorLog->GetBufferSize();
				char* compileErrorBuffer = new char[compileErrorBufferSize + 1];
				std::memcpy(compileErrorBuffer, errorLog->GetBufferPointer(), compileErrorBufferSize);
				compileErrorBuffer[compileErrorBufferSize] = 0;
				ss << compileErrorBuffer;
				m_compileError = ss.str();
				delete[] compileErrorBuffer;
				return false;
			}

#ifdef DEBUG
			m_source = buffer;
#endif
			m_name = file->GetName();
			delete[] buffer;
			if (pMacros)
			{
				for (int i = 0; i < macroCount; i++)
				{
					delete[] pMacros[i].Name;
					delete[] pMacros[i].Definition;
				}
				delete[] pMacros;
			}
			return true;
		}

		const std::string D3D12Shader::GetCompileErrorLog()const
		{
			return m_compileError;
		}

		std::string D3D12Shader::GetName()const
		{
			return m_name;
		}

#ifdef DEBUG
		std::string D3D12Shader::GetSource()const
		{
			return m_source;
		}
#endif


		void D3D12Shader::ReleaseRenderResources()
		{
			m_byteCode.Reset();
		}

		void D3D12Shader::GetShaderModelVersion(int& major, int& minor)
		{
			major = m_smMajorVersion;
			minor = m_smMinorVersion;
		}

		void D3D12Shader::GetShaderModelString(char* buf)const
		{
			std::stringstream ss;
			switch (m_type)
			{
			case SHADER_TYPE_VERTEX:
				ss << "vs";
				break;
			case SHADER_TYPE_FRAGMENT:
				ss << "ps";
				break;
			case SHADER_TYPE_GEOMETRY:
				ss << "gs";
				break;
			case SHADER_TYPE_HULL:
				ss << "hs";
				break;
			case SHADER_TYPE_DOMAIN:
				ss << "ds";
				break;
			}
			ss << "_" << m_smMajorVersion << "_" << m_smMinorVersion;
			std::strcpy(buf, ss.str().c_str());
		}

	}
}