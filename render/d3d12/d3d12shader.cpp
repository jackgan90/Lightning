#include <sstream>
#include <d3dx12.h>
#include <cassert>
#include "d3d12device.h"
#include "renderer.h"
#include "d3d12shader.h"
#include "rendererhelper.h"
#include "framememoryallocator.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::FileSize;
		using Foundation::FilePointerType;
		using Foundation::FileAnchor;
		using Foundation::FrameMemoryAllocator;

		extern FrameMemoryAllocator g_RenderAllocator;

		D3D12Shader::D3D12Shader(ID3D12Device* device, ShaderType type, const std::string& name, const std::string& entry, const char* const shaderSource):
			Shader(type, name, entry, shaderSource)
			, mDescriptorRanges(nullptr), mTotalConstantBufferSize(0)
		{
			assert(shaderSource);
			CompileImpl();
			ComPtr<ID3D12ShaderReflection> shaderReflection;
			D3DReflect(mByteCode->GetBufferPointer(), mByteCode->GetBufferSize(), IID_PPV_ARGS(&shaderReflection));
			shaderReflection->GetDesc(&mDesc);
			Container::UnorderedMap<std::string, D3D12_SHADER_INPUT_BIND_DESC> inputBindDescs;
			for (UINT i = 0;i < mDesc.BoundResources;++i)
			{
				D3D12_SHADER_INPUT_BIND_DESC bindDesc;
				shaderReflection->GetResourceBindingDesc(i, &bindDesc);
				inputBindDescs[bindDesc.Name] = bindDesc;
			}
			//create heap descriptor(samplers excluded)
			//TODO : should create sampler descriptor heap
			if (mDesc.ConstantBuffers > 0)
			{
				mDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[mDesc.ConstantBuffers];
				//initialize cbv descriptor ranges, number of descriptors is the number of constant buffers
				for (UINT i = 0; i < mDesc.ConstantBuffers; i++)
				{
					auto constantBufferRefl = shaderReflection->GetConstantBufferByIndex(i);
					D3D12_SHADER_BUFFER_DESC bufferDesc;
					constantBufferRefl->GetDesc(&bufferDesc);
					ConstantBufferInfo cbufferInfo;
					cbufferInfo.offset = mTotalConstantBufferSize;
					cbufferInfo.size = bufferDesc.Size;
					mConstantBufferInfo[i] = cbufferInfo;
					mTotalConstantBufferSize += bufferDesc.Size;
					for (UINT j = 0; j < bufferDesc.Variables; j++)
					{
						ID3D12ShaderReflectionVariable* variableRefl = constantBufferRefl->GetVariableByIndex(j);
						D3D12_SHADER_VARIABLE_DESC shaderVarDesc;
						variableRefl->GetDesc(&shaderVarDesc);
						ArgumentInfo info;
						info.bufferIndex = i;
						info.offsetInBuffer = shaderVarDesc.StartOffset;
						mArguments[shaderVarDesc.Name] = info;
					}
					D3D12_SHADER_INPUT_BIND_DESC& bindDesc = inputBindDescs[bufferDesc.Name];
					mDescriptorRanges[i].BaseShaderRegister = bindDesc.BindPoint;
					mDescriptorRanges[i].NumDescriptors = 1;
					mDescriptorRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
					mDescriptorRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					mDescriptorRanges[i].RegisterSpace = bindDesc.Space;
				}
				if (mTotalConstantBufferSize > 0)
				{
					mTotalConstantBufferSize = UINT(D3D12ConstantBufferManager::AlignedSize(mTotalConstantBufferSize));
				}
				CD3DX12_ROOT_PARAMETER cbvParameter;
				cbvParameter.InitAsDescriptorTable(mDesc.ConstantBuffers, mDescriptorRanges, GetParameterVisibility());
				mRootParameters.push_back(cbvParameter);
			}
			//TODO initialize other shader resource
		}

		D3D12Shader::~D3D12Shader()
		{
			if (mDescriptorRanges)
			{
				delete[] mDescriptorRanges;
			}
			mArguments.clear();
			mByteCode.Reset();
		}

		D3D12Shader::ShaderResourceProxy::ShaderResourceProxy():mConstantBuffer(nullptr), mBufferSize(0)
		{
		}

		std::uint8_t* D3D12Shader::ShaderResourceProxy::GetConstantBuffer(std::size_t size)
		{
			if (mConstantBuffer && mBufferSize >= size)
				return mConstantBuffer;
			if (mConstantBuffer)
				delete[] mConstantBuffer;
			mConstantBuffer = new std::uint8_t[size];
			mBufferSize = size;

			return mConstantBuffer;
		}

		Container::Vector<D3D12RootBoundResource>& D3D12Shader::ShaderResourceProxy::GetRootBoundResources()
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			return mRootBoundResources[resourceIndex];
		}

		D3D12Shader::ShaderResourceProxy::~ShaderResourceProxy()
		{
			if (mConstantBuffer)
				delete[] mConstantBuffer;
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

		bool D3D12Shader::SetArgument(const ShaderArgument& arg)
		{
			if (arg.type == ShaderArgumentType::UNKNOWN)
			{
				LOG_WARNING("Unknown shader argument type when set shader {0}", mName.c_str());
				return false;
			}
			auto it = mArguments.find(arg.name);
			assert(it != mArguments.end());
			const auto& bindingInfo = it->second;
			std::size_t size{ 0 };
			auto data = arg.Buffer(size);
			if (data)
			{
				std::uint8_t *p = mResourceProxy->GetConstantBuffer(mTotalConstantBufferSize);
				std::uint8_t *buffer = p + mConstantBufferInfo[bindingInfo.bufferIndex].offset;
				std::memcpy(buffer + bindingInfo.offsetInBuffer, data, size);
				return true;
			}
			return false;
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
			auto ptr = mResourceProxy->GetConstantBuffer(mTotalConstantBufferSize);
			auto& rootBoundResources = mResourceProxy->GetRootBoundResources();
			rootBoundResources.clear();
			D3D12RootBoundResource boundResource;
			boundResource.type = D3D12RootResourceType::ConstantBuffers;
			for (std::size_t i = 0;i < mDesc.ConstantBuffers;++i)
			{
				auto bufferSize = mConstantBufferInfo[i].size;
				auto cbuffer = D3D12ConstantBufferManager::Instance()->AllocBuffer(bufferSize);
				std::memcpy(cbuffer.userMemory, ptr + mConstantBufferInfo[i].offset, bufferSize);

				boundResource.buffers.push_back(cbuffer);
			}
			rootBoundResources.push_back(boundResource);
		}

		const Container::Vector<D3D12_ROOT_PARAMETER>& D3D12Shader::GetRootParameters()const
		{
			return mRootParameters;
		}

		std::size_t D3D12Shader::GetRootParameterCount()const
		{
			return mRootParameters.size();
		}


		const Container::Vector<D3D12RootBoundResource>& D3D12Shader::GetRootBoundResources()
		{
			UpdateRootBoundResources();
			return mResourceProxy->GetRootBoundResources();
		}

		UINT D3D12Shader::GetConstantBufferCount()
		{
			return mDesc.ConstantBuffers;
		}

		UINT D3D12Shader::GetConstantBufferSize()
		{
			return mTotalConstantBufferSize;
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
					auto nameSize = std::strlen(name) + 1;
					pMacros[idx].Name = g_RenderAllocator.Allocate<const char>(nameSize);
#ifdef _MSC_VER
					strcpy_s(const_cast<char*>(pMacros[idx].Name), nameSize, name);
#else
					std::strcpy(const_cast<char*>(pMacros[idx].Name), name);
#endif
					const char* definition = it->second.c_str();
					auto definitionSize = std::strlen(definition) + 1;
					pMacros[idx].Definition = g_RenderAllocator.Allocate<const char>(definitionSize);
#ifdef _MSC_VER
					strcpy_s(const_cast<char*>(pMacros[idx].Definition), definitionSize,definition);
#else
					std::strcpy(const_cast<char*>(pMacros[idx].Definition), definition);
#endif
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