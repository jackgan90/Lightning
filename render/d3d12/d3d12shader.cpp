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
			, mDescriptorRanges(nullptr)
		{
			assert(shaderSource);
			CompileImpl();
			D3DReflect(mByteCode->GetBufferPointer(), mByteCode->GetBufferSize(), IID_PPV_ARGS(&mShaderReflect));
			mShaderReflect->GetDesc(&mDesc);
			for (std::size_t i = 0;i < mDesc.BoundResources;++i)
			{
				D3D12_SHADER_INPUT_BIND_DESC bindDesc;
				mShaderReflect->GetResourceBindingDesc(i, &bindDesc);
				mInputBindDescs[bindDesc.Name] = bindDesc;
			}
			//create heap descriptor(samplers excluded)
			//TODO : should create sampler descriptor heap
			if (mDesc.ConstantBuffers > 0)
			{
				mDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[mDesc.ConstantBuffers * RENDER_FRAME_COUNT];
				//initialize cbv descriptor ranges, number of descriptors is the number of constant buffers
				mConstantHeap = D3D12DescriptorHeapManager::Instance()->Allocate(
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, mDesc.ConstantBuffers * RENDER_FRAME_COUNT, device);
				for (size_t i = 0; i < mDesc.ConstantBuffers; i++)
				{
					auto constantBufferRefl = mShaderReflect->GetConstantBufferByIndex(i);
					D3D12_SHADER_BUFFER_DESC bufferDesc;
					constantBufferRefl->GetDesc(&bufferDesc);
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
				}
			}
			for (std::size_t i = 0; i < RENDER_FRAME_COUNT;++i)
			{
				mRootBoundResources.emplace(i, container::vector<D3D12RootBoundResource>());
				if (mDesc.ConstantBuffers > 0)
				{
					CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mConstantHeap->cpuHandle);
					handle.Offset(i * mDesc.ConstantBuffers * mConstantHeap->incrementSize);
					CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(mConstantHeap->gpuHandle);
					gpuHandle.Offset(i * mDesc.ConstantBuffers * mConstantHeap->incrementSize);
					for (std::size_t k = 0;k < mDesc.ConstantBuffers;++k)
					{
						D3D12RootBoundResource boundResource;
						boundResource.descriptorTableHeap = D3D12DescriptorHeapManager::Instance()->GetHeap(mConstantHeap);
						boundResource.type = D3D12RootBoundResourceType::DescriptorTable;
						boundResource.descriptorTableHandle = gpuHandle;
						mRootBoundResources[i].push_back(boundResource);
						handle.Offset(mConstantHeap->incrementSize);
						gpuHandle.Offset(mConstantHeap->incrementSize);
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
			mArgumentBindings.clear();
			mByteCode.Reset();
			mShaderReflect.Reset();
			if (mDesc.ConstantBuffers > 0)
			{
				D3D12DescriptorHeapManager::Instance()->Deallocate(mConstantHeap);
				mConstantHeap = nullptr;
			}
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
					std::uint8_t *addr{ nullptr };
					std::size_t bufferId{ 0 };
					std::size_t size{ 0 };
					if (argument.type == ShaderArgumentType::FLOAT)
					{
						bufferId = D3D12ConstantBufferManager::Instance()->AllocBuffer(sizeof(float));
						addr = D3D12ConstantBufferManager::Instance()->LockBuffer(bufferId);
						*reinterpret_cast<float*>(addr + bindingInfo.offsetInBuffer) = argument.GetFloat();
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
						bufferId = D3D12ConstantBufferManager::Instance()->AllocBuffer(size);
						addr = D3D12ConstantBufferManager::Instance()->LockBuffer(bufferId);
						std::memcpy(addr + bindingInfo.offsetInBuffer, data, size);
					}
					D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
					cbvDesc.BufferLocation = D3D12ConstantBufferManager::Instance()->GetVirtualAddress(bufferId);
					cbvDesc.SizeInBytes = D3D12ConstantBufferManager::Instance()->GetBufferSize(bufferId);
					auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
					auto nativeDevice = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice())->GetNative();
					CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mConstantHeap->cpuHandle);
					handle.Offset((resourceIndex * mDesc.ConstantBuffers + bindingInfo.bufferIndex) * mConstantHeap->incrementSize);
					nativeDevice->CreateConstantBufferView(&cbvDesc, handle);
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

		void D3D12Shader::UpdateRootParameters()
		{
			static std::uint64_t lastUpdateFrame{ 0 };
			auto currentFrame = Renderer::Instance()->GetCurrentFrameCount();
			if (currentFrame > lastUpdateFrame)
			{
				lastUpdateFrame = currentFrame;
				if (mDescriptorRanges)
				{
					auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
					for (size_t i = 0; i < mDesc.ConstantBuffers; i++)
					{
						auto rangeIndex = resourceIndex * RENDER_FRAME_COUNT + i;
						D3D12_SHADER_INPUT_BIND_DESC& bindDesc = mInputBindDescs[mBufferDescs[i].Name];
						mDescriptorRanges[rangeIndex].BaseShaderRegister = bindDesc.BindPoint;
						mDescriptorRanges[rangeIndex].NumDescriptors = 1;
						mDescriptorRanges[rangeIndex].OffsetInDescriptorsFromTableStart = mConstantHeap->offsetInDescriptors + rangeIndex;
						mDescriptorRanges[rangeIndex].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
						mDescriptorRanges[rangeIndex].RegisterSpace = bindDesc.Space;
					}
					mRootParameters.clear();
					CD3DX12_ROOT_PARAMETER cbvParameter;
					cbvParameter.InitAsDescriptorTable(mDesc.ConstantBuffers, &mDescriptorRanges[resourceIndex * RENDER_FRAME_COUNT], GetParameterVisibility());
					mRootParameters.push_back(cbvParameter);
				}
			}
		}

		const container::vector<D3D12_ROOT_PARAMETER>& D3D12Shader::GetRootParameters()
		{
			UpdateRootParameters();
			return mRootParameters;
		}

		std::size_t D3D12Shader::GetRootParameterCount()const
		{
			return mRootParameters.size();
		}


		const container::vector<D3D12RootBoundResource>& D3D12Shader::GetRootBoundResources()const
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