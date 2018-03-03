#include <sstream>
#include <d3dx12.h>
#include <cassert>
#include "d3d12device.h"
#include "renderer.h"
#include "shadermanager.h"
#include "d3d12shader.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::FileSize;
		using Foundation::FilePointerType;
		using Foundation::FileAnchor;

		D3D12Shader::D3D12Shader(ID3D12Device* device, ShaderType type, const std::string& name, const std::string& entry, const char* const shaderSource):
			Shader(type, name, entry, shaderSource), m_commitHeapInfo(nullptr)
		{
			assert(shaderSource);
			CompileImpl(&s_compileAllocator);
			D3DReflect(m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize(), IID_PPV_ARGS(&m_shaderReflect));
			m_shaderReflect->GetDesc(&m_desc);
			//create heap descriptor(samplers excluded)
			//TODO : should create sampler descriptor heap
			if (m_desc.ConstantBuffers > 0)
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc{};
				desc.NumDescriptors = m_desc.BoundResources;
				desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				desc.NodeMask = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				m_commitHeapInfo = D3D12DescriptorHeapManager::Instance()->Create(desc, device);
				CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_commitHeapInfo->cpuHeapStart);
				for (size_t i = 0; i < m_desc.ConstantBuffers; i++)
				{
					auto constantBufferRefl = m_shaderReflect->GetConstantBufferByIndex(i);
					D3D12_SHADER_BUFFER_DESC bufferDesc;
					constantBufferRefl->GetDesc(&bufferDesc);
					auto bufferSize = bufferDesc.Size;
					//d3d12 constant buffer must be a multiple of 256,so we round it to the nearest larger multiple of 256
					bufferSize = (bufferSize + 255) & ~255;
					ConstantUploadContext context;
					context.handle = handle;
					context.bufferIndex = i;
					if (bufferDesc.Name)
					{
						context.bufferName = new char[std::strlen(bufferDesc.Name) + 1];
						std::strcpy(context.bufferName, bufferDesc.Name);
					}
					else
					{
						context.bufferName = nullptr;
					}
					for (size_t j = 0; j < bufferDesc.Variables; j++)
					{
						ID3D12ShaderReflectionVariable* variableRefl = constantBufferRefl->GetVariableByIndex(j);
						D3D12_SHADER_VARIABLE_DESC shaderVarDesc;
						variableRefl->GetDesc(&shaderVarDesc);
						m_constantNameToBuffers[shaderVarDesc.Name] = i;
					}
					device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
						&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
						D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&context.resource));
					D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
					cbvDesc.BufferLocation = context.resource->GetGPUVirtualAddress();
					cbvDesc.SizeInBytes = bufferSize;
					device->CreateConstantBufferView(&cbvDesc, context.handle);
					handle.Offset(m_commitHeapInfo->incrementSize);
					m_uploadContexts.push_back(context);
				}
				
			}
		}

		D3D12Shader::~D3D12Shader()
		{
			if (m_commitHeapInfo)
			{
				D3D12DescriptorHeapManager::Instance()->Destroy(m_commitHeapInfo->heapID);
				m_commitHeapInfo = nullptr;
			}
			for (auto& context : m_uploadContexts)
			{
				if (context.bufferName)
					delete[] context.bufferName;
			}
			m_uploadContexts.clear();
			m_constantNameToBuffers.clear();
			m_byteCode.Reset();
			m_shaderReflect.Reset();
		}

		const ShaderDefine D3D12Shader::GetMacros()const
		{
			return m_macros;
		}

		void* D3D12Shader::GetByteCodeBuffer()const
		{
			if (m_byteCode)
			{
				return m_byteCode->GetBufferPointer();
			}
			return nullptr;
		}

		SIZE_T D3D12Shader::GetByteCodeBufferSize()const
		{
			if (m_byteCode)
			{
				return m_byteCode->GetBufferSize();
			}
			return 0;
		}

		std::size_t D3D12Shader::GetArgumentCount()const
		{
			return m_desc.BoundResources;
		}

		void D3D12Shader::SetArgument(const ShaderArgument& argument)
		{

		}

		void D3D12Shader::Compile()
		{
			if (m_source)
			{
				CompileImpl(&s_compileAllocator);
			}
		}

		void D3D12Shader::CompileImpl(IMemoryAllocator* memoryAllocator)
		{
			D3D_SHADER_MACRO* pMacros = nullptr;
			auto macroCount = m_macros.GetMacroCount();
			if (macroCount)
			{
				pMacros = ALLOC_ARRAY(memoryAllocator, macroCount + 1, D3D_SHADER_MACRO);
				std::memset(&pMacros[macroCount], 0, sizeof(D3D_SHADER_MACRO));
				auto macros = m_macros.GetAllDefine();
				auto idx = 0;
				for (auto it = macros.begin(); it != macros.end(); ++it,++idx)
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
			m_smMajorVersion = DEFAULT_SHADER_MODEL_MAJOR_VERSION;
			m_smMinorVersion = DEFAULT_SHADER_MODEL_MINOR_VERSION;
			GetShaderModelString(shaderModel, m_type, DEFAULT_SHADER_MODEL_MAJOR_VERSION, DEFAULT_SHADER_MODEL_MINOR_VERSION);
			HRESULT hr = ::D3DCompile(m_source, static_cast<SIZE_T>(strlen(m_source) + 1), nullptr, pMacros, nullptr, DEFAULT_SHADER_ENTRY,
				shaderModel, flags1, flags2, &m_byteCode, &errorLog);
			if (FAILED(hr))
			{
				std::stringstream ss;
				ss << "Compile shader " << m_name << " failed!";
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
				logger.Log(LogLevel::Error, "%s", ss.str().c_str());
				DEALLOC(memoryAllocator, compileErrorBuffer);
				throw ShaderCompileException("Failed to compile shader!");
			}

			if (pMacros)
			{
				for (size_t i = 0; i < macroCount; i++)
				{
					memoryAllocator->Deallocate(static_cast<void*>(const_cast<char*>(pMacros[i].Name)));
					memoryAllocator->Deallocate(static_cast<void*>(const_cast<char*>(pMacros[i].Definition)));
				}
				memoryAllocator->Deallocate(pMacros);
			}
		}
	}
}