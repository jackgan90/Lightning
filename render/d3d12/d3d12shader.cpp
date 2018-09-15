#include <sstream>
#include <d3dx12.h>
#include <cassert>
#include "d3d12device.h"
#include "renderer.h"
#include "shadermanager.h"
#include "d3d12shader.h"
#include "rendererhelper.h"
#include "ringallocator.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::FileSize;
		using Foundation::FilePointerType;
		using Foundation::FileAnchor;
		using Foundation::RingAllocator;

		extern RingAllocator g_RenderAllocator;

		D3D12Shader::D3D12Shader(ID3D12Device* device, ShaderType type, const std::string& name, const std::string& entry, const char* const shaderSource):
			Shader(type, name, entry, shaderSource)
			, m_descriptorRanges(nullptr)
		{
			assert(shaderSource);
			CompileImpl();
			D3DReflect(m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize(), IID_PPV_ARGS(&m_shaderReflect));
			m_shaderReflect->GetDesc(&m_desc);
			std::unordered_map<std::string, D3D12_SHADER_INPUT_BIND_DESC> bindDescs;
			for (std::size_t i = 0;i < m_desc.BoundResources;++i)
			{
				D3D12_SHADER_INPUT_BIND_DESC bindDesc;
				m_shaderReflect->GetResourceBindingDesc(i, &bindDesc);
				bindDescs[bindDesc.Name] = bindDesc;
			}
			//create heap descriptor(samplers excluded)
			//TODO : should create sampler descriptor heap
			if (m_desc.ConstantBuffers > 0)
			{
				m_descriptorRanges = new D3D12_DESCRIPTOR_RANGE[m_desc.ConstantBuffers];
				//initialize cbv descriptor ranges, number of descriptors is the number of constant buffers
				m_constantHeap = D3D12DescriptorHeapManager::Instance()->Allocate(
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, m_desc.ConstantBuffers * RENDER_FRAME_COUNT, device);
				for (size_t i = 0; i < m_desc.ConstantBuffers; i++)
				{
					auto constantBufferRefl = m_shaderReflect->GetConstantBufferByIndex(i);
					D3D12_SHADER_BUFFER_DESC bufferDesc;
					constantBufferRefl->GetDesc(&bufferDesc);
					for (size_t j = 0; j < bufferDesc.Variables; j++)
					{
						ID3D12ShaderReflectionVariable* variableRefl = constantBufferRefl->GetVariableByIndex(j);
						D3D12_SHADER_VARIABLE_DESC shaderVarDesc;
						variableRefl->GetDesc(&shaderVarDesc);
						ArgumentBinding argBinding;
						argBinding.bufferIndex = i;
						argBinding.offsetInBuffer = shaderVarDesc.StartOffset;
						m_argumentBindings[shaderVarDesc.Name] = argBinding;
					}
					//TODO : now one constant buffer corresponds to one descriptor range.I believe this is 
					//not very efficient.Should join adjacent constant buffers to one range
					D3D12_SHADER_INPUT_BIND_DESC& bindDesc = bindDescs[bufferDesc.Name];
					m_descriptorRanges[i].BaseShaderRegister = bindDesc.BindPoint;
					m_descriptorRanges[i].NumDescriptors = 1;
					m_descriptorRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
					m_descriptorRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					m_descriptorRanges[i].RegisterSpace = bindDesc.Space;

					ConstantUploadContext context;
					context.bufferName = new char[std::strlen(bufferDesc.Name) + 1];
					std::strcpy(context.bufferName, bufferDesc.Name);
					context.registerIndex = bindDesc.BindPoint;
					context.bufferSize = bufferDesc.Size;
					m_uploadContexts.push_back(context);
				}
				CD3DX12_ROOT_PARAMETER cbvParameter;
				cbvParameter.InitAsDescriptorTable(m_desc.ConstantBuffers, m_descriptorRanges, GetParameterVisibility());
				m_rootParameters.push_back(cbvParameter);
			}
			for (std::size_t i = 0; i < RENDER_FRAME_COUNT;++i)
			{
				m_rootBoundResources.emplace(i, std::vector<D3D12RootBoundResource>());
				if (m_desc.ConstantBuffers > 0)
				{
					CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_constantHeap.cpuHandle);
					handle.Offset(i * m_desc.ConstantBuffers * m_constantHeap.incrementSize);
					CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_constantHeap.gpuHandle);
					gpuHandle.Offset(i * m_desc.ConstantBuffers * m_constantHeap.incrementSize);
					for (std::size_t k = 0;k < m_desc.ConstantBuffers;++k)
					{
						auto& context = m_uploadContexts[k];
						context.handle[i] = handle;
						//d3d12 constant buffer must be a multiple of 256,so we round it to the nearest larger multiple of 256
						UINT64 bufferSize = (context.bufferSize + 255) & ~255;
						device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
							&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
							D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&context.resource[i]));
						D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
						cbvDesc.BufferLocation = context.resource[i]->GetGPUVirtualAddress();
						cbvDesc.SizeInBytes = bufferSize;
						device->CreateConstantBufferView(&cbvDesc, context.handle[i]);
						D3D12RootBoundResource boundResource;
						boundResource.descriptorTableHeap = D3D12DescriptorHeapManager::Instance()->GetHeap(m_constantHeap.heapID);
						boundResource.type = D3D12RootBoundResourceType::DescriptorTable;
						boundResource.descriptorTableHandle = gpuHandle;
						m_rootBoundResources[i].push_back(boundResource);
						handle.Offset(m_constantHeap.incrementSize);
						gpuHandle.Offset(m_constantHeap.incrementSize);
					}
				}
			}
			//TODO initialize other shader resource
		}

		D3D12Shader::~D3D12Shader()
		{
			if (m_descriptorRanges)
			{
				delete[] m_descriptorRanges;
			}
			for (auto& context : m_uploadContexts)
			{
				if (context.bufferName)
				{
					delete[] context.bufferName;
				}
				for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					context.resource[i].Reset();
				}
			}
			m_uploadContexts.clear();
			m_argumentBindings.clear();
			m_byteCode.Reset();
			m_shaderReflect.Reset();
			if(m_desc.ConstantBuffers > 0)
				D3D12DescriptorHeapManager::Instance()->Deallocate(m_constantHeap.cpuHandle);
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
			if (argument.type == ShaderArgumentType::UNKNOWN)
			{
				logger.Log(LogLevel::Warning, "Unknown shader argument type when set shader %s", m_name.c_str());
				return;
			}
			switch (argument.type)
			{
			case ShaderArgumentType::FLOAT:
			case ShaderArgumentType::FLOAT2:
			case ShaderArgumentType::FLOAT3:
			case ShaderArgumentType::FLOAT4:
			case ShaderArgumentType::MATRIX2:
			case ShaderArgumentType::MATRIX3:
			case ShaderArgumentType::MATRIX4:
			{
				//assert(m_commitHeapInfo);
				auto it = m_argumentBindings.find(argument.name);
				if (it == m_argumentBindings.end())
				{
					//logger.Log(LogLevel::Warning, "shader argument of type %d with name %s doesn't exist in shader %s",
					//	argument.type, argument.name.c_str(), m_name.c_str());
				}
				else
				{
					const auto& bindingInfo = it->second;
					auto& uploadContext = m_uploadContexts[bindingInfo.bufferIndex];
					CD3DX12_RANGE readRange(0, 0);
					D3D12_RANGE writtenRange{};
					void* temp{ nullptr };
					auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
					uploadContext.resource[resourceIndex]->Map(0, &readRange, &temp);
					void* mappedStart = reinterpret_cast<std::uint8_t*>(temp) + bindingInfo.offsetInBuffer;
					writtenRange.Begin = bindingInfo.offsetInBuffer;
					if (argument.type == ShaderArgumentType::FLOAT)
					{
						*reinterpret_cast<float*>(mappedStart) = argument.GetFloat();
						writtenRange.End = writtenRange.Begin + sizeof(float);
					}
					else
					{
						const float* data{ nullptr };
						std::size_t size{ 0 };
						switch (argument.type)
						{
						case ShaderArgumentType::FLOAT2:
							data = argument.GetVector2().GetData();
							size = 2 * sizeof(float);
							break;
						case ShaderArgumentType::FLOAT3:
							data = argument.GetVector3().GetData();
							size = 3 * sizeof(float);
							break;
						case ShaderArgumentType::FLOAT4:
							data = argument.GetVector4().GetData();
							size = 4 * sizeof(float);
							break;
						case ShaderArgumentType::MATRIX2:
							data = argument.GetMatrix2().GetData();
							size = 4 * sizeof(float);
							break;
						case ShaderArgumentType::MATRIX3:
							data = argument.GetMatrix3().GetData();
							size = 9 * sizeof(float);
							break;
						case ShaderArgumentType::MATRIX4:
							data = argument.GetMatrix4().GetData();
							size = 16 * sizeof(float);
							break;
						default:
							break;
						}
						std::memcpy(mappedStart, data, size);
						writtenRange.End = writtenRange.Begin + size;
					}
					uploadContext.resource[resourceIndex]->Unmap(0, &writtenRange);
				}
				break;
			}
			default:
				break;
			}
		}

		void D3D12Shader::Compile()
		{
			if (m_source)
			{
				CompileImpl();
			}
		}

		const std::vector<D3D12_ROOT_PARAMETER>& D3D12Shader::GetRootParameters()const
		{
			return m_rootParameters;
		}

		std::size_t D3D12Shader::GetRootParameterCount()const
		{
			return m_rootParameters.size();
		}


		const std::vector<D3D12RootBoundResource>& D3D12Shader::GetRootBoundResources()const
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto it = m_rootBoundResources.find(resourceIndex);
			assert(it != m_rootBoundResources.end());
			return it->second;
		}

		D3D12_SHADER_VISIBILITY D3D12Shader::GetParameterVisibility()const
		{
			switch (m_type)
			{
			case ShaderType::VERTEX:
				return D3D12_SHADER_VISIBILITY_VERTEX;
			case ShaderType::FRAGMENT:
				return D3D12_SHADER_VISIBILITY_PIXEL;
			case ShaderType::GEOMETRY:
				return D3D12_SHADER_VISIBILITY_GEOMETRY;
			case ShaderType::TESSELATION_CONTROL:
				return D3D12_SHADER_VISIBILITY_HULL;
			case ShaderType::TESSELATION_EVALUATION:
				return D3D12_SHADER_VISIBILITY_DOMAIN;
			default:
				return D3D12_SHADER_VISIBILITY_ALL;
			}
		}



		void D3D12Shader::CompileImpl()
		{
			D3D_SHADER_MACRO* pMacros = nullptr;
			auto macroCount = m_macros.GetMacroCount();
			if (macroCount)
			{
				pMacros = g_RenderAllocator.Allocate<D3D_SHADER_MACRO>(macroCount + 1);
				std::memset(&pMacros[macroCount], 0, sizeof(D3D_SHADER_MACRO));
				auto macros = m_macros.GetAllDefine();
				auto idx = 0;
				for (auto it = macros.begin(); it != macros.end(); ++it,++idx)
				{
					const char* name = it->first.c_str();
					pMacros[idx].Name = g_RenderAllocator.Allocate<const char>(std::strlen(name) + 1);
					std::strcpy(const_cast<char*>(pMacros[idx].Name), name);
					const char* definition = it->second.c_str();
					pMacros[idx].Definition = g_RenderAllocator.Allocate<const char>(std::strlen(definition) + 1);
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
					}
				}
				ss << "Detailed info:" << std::endl;
				size_t compileErrorBufferSize = errorLog->GetBufferSize();
				char* compileErrorBuffer = g_RenderAllocator.Allocate<char>(compileErrorBufferSize);
				std::memcpy(compileErrorBuffer, errorLog->GetBufferPointer(), compileErrorBufferSize);
				compileErrorBuffer[compileErrorBufferSize] = 0;
				ss << compileErrorBuffer;
				logger.Log(LogLevel::Error, "%s", ss.str().c_str());
				throw ShaderCompileException("Failed to compile shader!");
			}
		}
	}
}