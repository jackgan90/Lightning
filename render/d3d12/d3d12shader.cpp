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
			, mDescriptorRanges(nullptr)
		{
			assert(shaderSource);
			CompileImpl();
			D3DReflect(mByteCode->GetBufferPointer(), mByteCode->GetBufferSize(), IID_PPV_ARGS(&mShaderReflect));
			mShaderReflect->GetDesc(&mDesc);
			std::unordered_map<std::string, D3D12_SHADER_INPUT_BIND_DESC> bindDescs;
			for (std::size_t i = 0;i < mDesc.BoundResources;++i)
			{
				D3D12_SHADER_INPUT_BIND_DESC bindDesc;
				mShaderReflect->GetResourceBindingDesc(i, &bindDesc);
				bindDescs[bindDesc.Name] = bindDesc;
			}
			//create heap descriptor(samplers excluded)
			//TODO : should create sampler descriptor heap
			if (mDesc.ConstantBuffers > 0)
			{
				mDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[mDesc.ConstantBuffers];
				//initialize cbv descriptor ranges, number of descriptors is the number of constant buffers
				mConstantHeap = D3D12DescriptorHeapManager::Instance()->Allocate(
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, mDesc.ConstantBuffers * RENDER_FRAME_COUNT, device);
				for (size_t i = 0; i < mDesc.ConstantBuffers; i++)
				{
					auto constantBufferRefl = mShaderReflect->GetConstantBufferByIndex(i);
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
						mArgumentBindings[shaderVarDesc.Name] = argBinding;
					}
					//TODO : now one constant buffer corresponds to one descriptor range.I believe this is 
					//not very efficient.Should join adjacent constant buffers to one range
					D3D12_SHADER_INPUT_BIND_DESC& bindDesc = bindDescs[bufferDesc.Name];
					mDescriptorRanges[i].BaseShaderRegister = bindDesc.BindPoint;
					mDescriptorRanges[i].NumDescriptors = 1;
					mDescriptorRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
					mDescriptorRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					mDescriptorRanges[i].RegisterSpace = bindDesc.Space;

					ConstantUploadContext context;
					context.bufferName = new char[std::strlen(bufferDesc.Name) + 1];
					std::strcpy(context.bufferName, bufferDesc.Name);
					context.registerIndex = bindDesc.BindPoint;
					context.bufferSize = bufferDesc.Size;
					mUploadContexts.push_back(context);
				}
				CD3DX12_ROOT_PARAMETER cbvParameter;
				cbvParameter.InitAsDescriptorTable(mDesc.ConstantBuffers, mDescriptorRanges, GetParameterVisibility());
				mRootParameters.push_back(cbvParameter);
			}
			for (std::size_t i = 0; i < RENDER_FRAME_COUNT;++i)
			{
				mRootBoundResources.emplace(i, std::vector<D3D12RootBoundResource>());
				if (mDesc.ConstantBuffers > 0)
				{
					CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mConstantHeap.cpuHandle);
					handle.Offset(i * mDesc.ConstantBuffers * mConstantHeap.incrementSize);
					CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(mConstantHeap.gpuHandle);
					gpuHandle.Offset(i * mDesc.ConstantBuffers * mConstantHeap.incrementSize);
					for (std::size_t k = 0;k < mDesc.ConstantBuffers;++k)
					{
						auto& context = mUploadContexts[k];
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
						boundResource.descriptorTableHeap = D3D12DescriptorHeapManager::Instance()->GetHeap(mConstantHeap.heapID);
						boundResource.type = D3D12RootBoundResourceType::DescriptorTable;
						boundResource.descriptorTableHandle = gpuHandle;
						mRootBoundResources[i].push_back(boundResource);
						handle.Offset(mConstantHeap.incrementSize);
						gpuHandle.Offset(mConstantHeap.incrementSize);
					}
				}
			}
			//TODO initialize other shader resource
		}

		D3D12Shader::~D3D12Shader()
		{
			if (mDescriptorRanges)
			{
				delete[] mDescriptorRanges;
			}
			for (auto& context : mUploadContexts)
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
			mUploadContexts.clear();
			mArgumentBindings.clear();
			mByteCode.Reset();
			mShaderReflect.Reset();
			if(mDesc.ConstantBuffers > 0)
				D3D12DescriptorHeapManager::Instance()->Deallocate(mConstantHeap.cpuHandle);
		}

		const ShaderDefine D3D12Shader::GetMacros()const
		{
			return mMacros;
		}

		void* D3D12Shader::GetByteCodeBuffer()const
		{
			if (mByteCode)
			{
				return mByteCode->GetBufferPointer();
			}
			return nullptr;
		}

		SIZE_T D3D12Shader::GetByteCodeBufferSize()const
		{
			if (mByteCode)
			{
				return mByteCode->GetBufferSize();
			}
			return 0;
		}

		std::size_t D3D12Shader::GetArgumentCount()const
		{
			return mDesc.BoundResources;
		}

		void D3D12Shader::SetArgument(const ShaderArgument& argument)
		{
			if (argument.type == ShaderArgumentType::UNKNOWN)
			{
				LOG_WARNING("Unknown shader argument type when set shader %s", mName.c_str());
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
				auto it = mArgumentBindings.find(argument.name);
				if (it == mArgumentBindings.end())
				{
					//logger.Log(LogLevel::Warning, "shader argument of type %d with name %s doesn't exist in shader %s",
					//	argument.type, argument.name.c_str(), mName.c_str());
				}
				else
				{
					const auto& bindingInfo = it->second;
					auto& uploadContext = mUploadContexts[bindingInfo.bufferIndex];
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
			if (mSource)
			{
				CompileImpl();
			}
		}

		const std::vector<D3D12_ROOT_PARAMETER>& D3D12Shader::GetRootParameters()const
		{
			return mRootParameters;
		}

		std::size_t D3D12Shader::GetRootParameterCount()const
		{
			return mRootParameters.size();
		}


		const std::vector<D3D12RootBoundResource>& D3D12Shader::GetRootBoundResources()const
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto it = mRootBoundResources.find(resourceIndex);
			assert(it != mRootBoundResources.end());
			return it->second;
		}

		D3D12_SHADER_VISIBILITY D3D12Shader::GetParameterVisibility()const
		{
			switch (mType)
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
			auto macroCount = mMacros.GetMacroCount();
			if (macroCount)
			{
				pMacros = g_RenderAllocator.Allocate<D3D_SHADER_MACRO>(macroCount + 1);
				std::memset(&pMacros[macroCount], 0, sizeof(D3D_SHADER_MACRO));
				auto macros = mMacros.GetAllMacros();
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
			mShaderModelMajorVersion = DEFAULT_SHADER_MODEL_MAJOR_VERSION;
			mShaderModelMinorVersion = DEFAULT_SHADER_MODEL_MINOR_VERSION;
			GetShaderModelString(shaderModel, mType, DEFAULT_SHADER_MODEL_MAJOR_VERSION, DEFAULT_SHADER_MODEL_MINOR_VERSION);
			HRESULT hr = ::D3DCompile(mSource, static_cast<SIZE_T>(strlen(mSource) + 1), nullptr, pMacros, nullptr, DEFAULT_SHADER_ENTRY,
				shaderModel, flags1, flags2, &mByteCode, &errorLog);
			if (FAILED(hr))
			{
				std::stringstream ss;
				ss << "Compile shader " << mName << " failed!";
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
				LOG_ERROR("%s", ss.str().c_str());
				throw ShaderCompileException("Failed to compile shader!");
			}
		}
	}
}