#include <d3dcompiler.h>
#include <sstream>
#include "shadermanager.h"
#include "d3d12shader.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::FileSize;
		using Foundation::FilePointerType;
		using Foundation::FileAnchor;

		D3D12Shader::D3D12Shader(ShaderType type, D3D12ShaderManager* manager):Shader(manager), m_type(type)
			,m_name(""), m_compileError(""), m_smMajorVersion(5), m_smMinorVersion(0)
#ifndef NDEBUG
			,m_source("")
#endif
		{

		}

		D3D12Shader::~D3D12Shader()
		{

		}


		std::string D3D12Shader::GetEntryPoint()const
		{
			return m_entryPoint;
		}

		ShaderType D3D12Shader::GetType()const
		{
			return m_type;
		}

		const ShaderDefine D3D12Shader::GetMacros()const
		{
			return m_macros;
		}

		bool D3D12Shader::Compile(const Foundation::SharedFilePtr& file, const ShaderDefine& define)
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
			auto memoryAllocator = m_shaderMgr->GetCompileAllocator();
			file->SetFilePointer(FilePointerType::Read, FileAnchor::Begin, 0);
			char* buffer = ALLOC_ARRAY(memoryAllocator, size+1, char);
			FileSize readSize = file->Read(buffer, size);
			if (readSize < size)
			{
				std::stringstream ss;
				ss << "Failed to read shader file.Shader name:" << file->GetPath() << ",file size:" << size << ",read size:" << readSize;
				m_compileError = ss.str();
				DEALLOC(memoryAllocator, buffer);
				return false;
			}
			buffer[size] = 0;
			D3D_SHADER_MACRO* pMacros = nullptr;
			size_t macroCount = define.GetMacroCount();
			if (macroCount)
			{
				pMacros = ALLOC_ARRAY(memoryAllocator, macroCount + 1, D3D_SHADER_MACRO);
				std::memset(&pMacros[macroCount], 0, sizeof(D3D_SHADER_MACRO));
				DefineMap defineMap = define.GetAllDefine();
				size_t idx = 0;
				for (auto it = defineMap.begin(); it != defineMap.end(); ++it,++idx)
				{
					const char* name = it->first.c_str();
					pMacros[idx].Name = ALLOC_ARRAY(memoryAllocator, std::strlen(name)+1, const char);
					std::strcpy(const_cast<char*>(pMacros[idx].Name), name);
					const char* definition = it->second.c_str();
					pMacros[idx].Definition = ALLOC_ARRAY(memoryAllocator, std::strlen(definition)+1, const char);
					std::strcpy(const_cast<char*>(pMacros[idx].Definition), definition);
				}
			}
			//TODO: resolve include
#ifndef NDEBUG
			UINT flags1 = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			UINT flags1 = 0;
#endif
			//TODO : flags2 is used to compile effect file.Should implement it later
			UINT flags2 = 0;
			ComPtr<ID3DBlob> errorLog;
			char shaderModel[32];
			GetShaderModelString(shaderModel);
			HRESULT hr = ::D3DCompile(buffer, static_cast<SIZE_T>(size + 1), nullptr, pMacros, nullptr, m_entryPoint.c_str(),
				shaderModel, flags1, flags2, &m_byteCode, &errorLog);
			if (FAILED(hr))
			{
				DEALLOC(memoryAllocator, buffer);
				std::stringstream ss;
				ss << "Compile shader " << file->GetPath() << " failed!";
				if (macroCount)
				{
					ss << "Defined macros:" << std::endl;
					for (size_t i = 0; i < macroCount; i++)
					{
						ss << pMacros[i].Name << ":" << pMacros[i].Definition << std::endl;
						DEALLOC(memoryAllocator, const_cast<char*>(pMacros[i].Name));
						DEALLOC(memoryAllocator, const_cast<char*>(pMacros[i].Definition));
					}
				}
				if (pMacros)
				{
					DEALLOC(memoryAllocator, pMacros);
				}
				ss << "Detailed info:" << std::endl;
				size_t compileErrorBufferSize = errorLog->GetBufferSize();
				char* compileErrorBuffer = ALLOC(memoryAllocator, compileErrorBufferSize, char);
				std::memcpy(compileErrorBuffer, errorLog->GetBufferPointer(), compileErrorBufferSize);
				compileErrorBuffer[compileErrorBufferSize] = 0;
				ss << compileErrorBuffer;
				m_compileError = ss.str();
				DEALLOC(memoryAllocator, compileErrorBuffer);
				return false;
			}

#ifndef NDEBUG
			m_source = buffer;
#endif
			m_name = file->GetName();
			memoryAllocator->Deallocate(buffer);
			if (pMacros)
			{
				for (size_t i = 0; i < macroCount; i++)
				{
					memoryAllocator->Deallocate(static_cast<void*>(const_cast<char*>(pMacros[i].Name)));
					memoryAllocator->Deallocate(static_cast<void*>(const_cast<char*>(pMacros[i].Definition)));
				}
				memoryAllocator->Deallocate(pMacros);
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

#ifndef NDEBUG
		std::string D3D12Shader::GetSource()const
		{
			return m_source;
		}
#endif

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

		void* D3D12Shader::GetByteCodeBuffer()const
		{
			if (m_byteCode)
			{
				m_byteCode->GetBufferPointer();
			}
			return nullptr;
		}

		SIZE_T D3D12Shader::GetByteCodeBufferSize()const
		{
			if (m_byteCode)
			{
				m_byteCode->GetBufferSize();
			}
			return 0;
		}

	}
}