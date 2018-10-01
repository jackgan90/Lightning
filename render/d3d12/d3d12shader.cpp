#include <sstream>
#include <d3dx12.h>
#include <cassert>
#include "d3d12device.h"
#include "renderer.h"
#include "shadermanager.h"
#include "d3d12shader.h"
#include "d3d12constantbuffermanager.h"
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
			, mDescriptorRanges(nullptr), mIntermediateBuffer(nullptr)
		{
			assert(shaderSource);
			CompileImpl();
			ComPtr<ID3D12ShaderReflection> shaderReflection;
			D3DReflect(mByteCode->GetBufferPointer(), mByteCode->GetBufferSize(), IID_PPV_ARGS(&shaderReflection));
			shaderReflection->GetDesc(&mDesc);
			for (std::size_t i = 0;i < mDesc.BoundResources;++i)
			{
				D3D12_SHADER_INPUT_BIND_DESC bindDesc;
				shaderReflection->GetResourceBindingDesc(i, &bindDesc);
				mInputBindDescs[bindDesc.Name] = bindDesc;
			}
			//create heap descriptor(samplers excluded)
			//TODO : should create sampler descriptor heap
			if (mDesc.ConstantBuffers > 0)
			{
				mDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[mDesc.ConstantBuffers];
				std::size_t totalBufferSize{ 0 };
				//initialize cbv descriptor ranges, number of descriptors is the number of constant buffers
				for (size_t i = 0; i < mDesc.ConstantBuffers; i++)
				{
					auto constantBufferRefl = shaderReflection->GetConstantBufferByIndex(i);
					D3D12_SHADER_BUFFER_DESC bufferDesc;
					constantBufferRefl->GetDesc(&bufferDesc);
					ConstantBufferInfo cbufferInfo;
					cbufferInfo.offset = totalBufferSize;
					cbufferInfo.size = bufferDesc.Size;
					mConstantBufferInfo[i] = cbufferInfo;
					totalBufferSize += bufferDesc.Size;
					mBufferDescs[i] = bufferDesc;
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
					D3D12_SHADER_INPUT_BIND_DESC& bindDesc = mInputBindDescs[bufferDesc.Name];
					mDescriptorRanges[i].BaseShaderRegister = bindDesc.BindPoint;
					mDescriptorRanges[i].NumDescriptors = 1;
					mDescriptorRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
					mDescriptorRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					mDescriptorRanges[i].RegisterSpace = bindDesc.Space;
				}
				CD3DX12_ROOT_PARAMETER cbvParameter;
				cbvParameter.InitAsDescriptorTable(mDesc.ConstantBuffers, mDescriptorRanges, GetParameterVisibility());
				mRootParameters.push_back(cbvParameter);
				mIntermediateBuffer = new std::uint8_t[totalBufferSize];
			}
			for (std::size_t i = 0; i < RENDER_FRAME_COUNT;++i)
			{
				mRootBoundResources.emplace(i, container::vector<D3D12RootBoundResource>());
			}
			//TODO initialize other shader resource
		}

		D3D12Shader::~D3D12Shader()
		{
			if (mDescriptorRanges)
			{
				delete[] mDescriptorRanges;
			}
			if (mIntermediateBuffer)
			{
				delete[] mIntermediateBuffer;
			}
			mArgumentBindings.clear();
			mByteCode.Reset();
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
					std::size_t size{ 0 };
					std::size_t bufferId{ 0 };
					std::uint8_t *buffer = mIntermediateBuffer + mConstantBufferInfo[bindingInfo.bufferIndex].offset;
					if (argument.type == ShaderArgumentType::FLOAT)
					{
						*reinterpret_cast<float*>(buffer + bindingInfo.offsetInBuffer) = argument.GetFloat();
					}
					else
					{
						float data[16];
						switch (argument.type)
						{
						case ShaderArgumentType::FLOAT2:
						{
							const auto v2 = argument.GetVector2();
							data[0] = v2.x;
							data[1] = v2.y;
							size = 2 * sizeof(float);
							break;
						}
						case ShaderArgumentType::FLOAT3:
						{
							const auto v3 = argument.GetVector3();
							data[0] = v3.x;
							data[1] = v3.y;
							data[2] = v3.z;
							size = 3 * sizeof(float);
							break;
						}
						case ShaderArgumentType::FLOAT4:
						{
							const auto v4 = argument.GetVector4();
							data[0] = v4.x;
							data[1] = v4.y;
							data[2] = v4.z;
							data[3] = v4.w;
							size = 4 * sizeof(float);
							break;
						}
						case ShaderArgumentType::MATRIX2:
							break;
						case ShaderArgumentType::MATRIX3:
						{
							break;
						}
						case ShaderArgumentType::MATRIX4:
						{
							size = 16 * sizeof(float);
							const auto m4 = argument.GetMatrix4().m;
							std::memcpy(data, m4, size);
							break;
						}
						default:
							break;
						}
						std::memcpy(buffer + bindingInfo.offsetInBuffer, data, size);
					}
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

		void D3D12Shader::UpdateRootBoundResources()
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			mRootBoundResources[resourceIndex].clear();
			auto constantHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				GetParameterVisibility(), mDesc.ConstantBuffers, true);
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(constantHeap->cpuHandle);
			for (std::size_t i = 0;i < mDesc.ConstantBuffers;++i)
			{
				auto bufferSize = mConstantBufferInfo[i].size;
				auto bufferId = D3D12ConstantBufferManager::Instance()->AllocBuffer(bufferSize);
				auto buffer = D3D12ConstantBufferManager::Instance()->LockBuffer(bufferId);
				std::memcpy(buffer, mIntermediateBuffer + mConstantBufferInfo[i].offset, bufferSize);

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
				cbvDesc.BufferLocation = D3D12ConstantBufferManager::Instance()->GetVirtualAddress(bufferId);
				cbvDesc.SizeInBytes = D3D12ConstantBufferManager::Instance()->GetBufferSize(bufferId);
				auto nativeDevice = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice())->GetNative();
				handle.Offset(i * constantHeap->incrementSize);
				nativeDevice->CreateConstantBufferView(&cbvDesc, handle);

				D3D12RootBoundResource boundResource;
				boundResource.type = D3D12RootBoundResourceType::DescriptorTable;
				boundResource.descriptorTableHeap = D3D12DescriptorHeapManager::Instance()->GetHeap(constantHeap);
				CD3DX12_GPU_DESCRIPTOR_HANDLE gpuAddress(constantHeap->gpuHandle);
				boundResource.descriptorTableHandle = gpuAddress;
				mRootBoundResources[resourceIndex].push_back(boundResource);
			}
		}

		const container::vector<D3D12_ROOT_PARAMETER>& D3D12Shader::GetRootParameters()const
		{
			return mRootParameters;
		}

		std::size_t D3D12Shader::GetRootParameterCount()const
		{
			return mRootParameters.size();
		}


		const container::vector<D3D12RootBoundResource>& D3D12Shader::GetRootBoundResources()
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto it = mRootBoundResources.find(resourceIndex);
			assert(it != mRootBoundResources.end());
			UpdateRootBoundResources();
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