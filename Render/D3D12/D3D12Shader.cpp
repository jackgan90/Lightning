#include <sstream>
#include <d3dx12.h>
#include <cassert>
#include "D3D12Device.h"
#include "Renderer.h"
#include "D3D12Shader.h"
#include "RendererHelper.h"
#include "FrameMemoryAllocator.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::FileSize;
		using Foundation::FilePointerType;
		using Foundation::FileAnchor;

		extern FrameMemoryAllocator g_RenderAllocator;

		D3D12Shader::D3D12Shader(D3D_SHADER_MODEL shaderModel, ShaderType type, const std::string& name, const char* const shaderSource):
			Shader(type, name, shaderSource)
			, mShaderModel(shaderModel), mTotalConstantBufferSize(0)
			, mTextureParameterCount(0), mSamplerStateParamCount(0)
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
			InitConstantBufferRootParameter(shaderReflection.Get(), inputBindDescs);
			InitTextureRootParameter(inputBindDescs);
			InitSamplerStateParameter(inputBindDescs);
		}

		void D3D12Shader::InitConstantBufferRootParameter(
			ID3D12ShaderReflection* shaderReflection, 
			const Container::UnorderedMap<std::string, D3D12_SHADER_INPUT_BIND_DESC>& inputBindDescs)
		{
			if (mDesc.ConstantBuffers > 0)
			{
				auto renderer = Renderer::Instance();
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
						ParameterInfo info;
						info.bufferIndex = i;
						info.offsetInBuffer = shaderVarDesc.StartOffset;
						mParameters[shaderVarDesc.Name] = info;
						auto semantic = renderer->GetUniformSemantic(shaderVarDesc.Name);
						if (semantic != RenderSemantics::UNKNOWN)
						{
							mUniformSemantics.push_back(semantic);
						}
					}
					const auto& bindDesc = inputBindDescs.at(bufferDesc.Name);
					D3D12_DESCRIPTOR_RANGE range;
					range.BaseShaderRegister = bindDesc.BindPoint;
					range.NumDescriptors = 1;
					range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
					range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					range.RegisterSpace = bindDesc.Space;
					mDescriptorRanges.push_back(range);
				}
				if (mTotalConstantBufferSize > 0)
				{
					mTotalConstantBufferSize = UINT(D3D12ConstantBufferManager::AlignedSize(mTotalConstantBufferSize));
				}
				CD3DX12_ROOT_PARAMETER cbvParameter;
				cbvParameter.InitAsDescriptorTable(mDesc.ConstantBuffers, &mDescriptorRanges[0], GetParameterVisibility());
				mRootParameters.push_back(cbvParameter);
			}
		}

		void D3D12Shader::InitTextureRootParameter(
			const Container::UnorderedMap<std::string, D3D12_SHADER_INPUT_BIND_DESC>& inputBindDescs)
		{
			auto resourceCount = mDescriptorRanges.size();
			for (auto it = inputBindDescs.cbegin(); it != inputBindDescs.cend();++it)
			{
				const auto& desc = it->second;
				if (desc.Type == D3D_SIT_TEXTURE)
				{
					D3D12_DESCRIPTOR_RANGE range;
					range.BaseShaderRegister = desc.BindPoint;
					range.NumDescriptors = 1;
					range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
					range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
					range.RegisterSpace = desc.Space;
					mDescriptorRanges.push_back(range);
					++mTextureParameterCount;
				}
			}
			if (mTextureParameterCount > 0)
			{
				CD3DX12_ROOT_PARAMETER textureParameter;
				textureParameter.InitAsDescriptorTable(mTextureParameterCount, &mDescriptorRanges[resourceCount], GetParameterVisibility());
				mRootParameters.push_back(textureParameter);
			}
		}

		void D3D12Shader::InitSamplerStateParameter(
			const Container::UnorderedMap<std::string, D3D12_SHADER_INPUT_BIND_DESC>& inputBindDescs)
		{
			auto resourceCount = mDescriptorRanges.size();
			for (auto it = inputBindDescs.cbegin(); it != inputBindDescs.cend();++it)
			{
				const auto& desc = it->second;
				if (desc.Type == D3D_SIT_SAMPLER)
				{
					D3D12_DESCRIPTOR_RANGE range;
					range.BaseShaderRegister = desc.BindPoint;
					range.NumDescriptors = 1;
					range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
					range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
					range.RegisterSpace = desc.Space;
					mDescriptorRanges.push_back(range);
					++mSamplerStateParamCount;
				}
			}
			if (mSamplerStateParamCount > 0)
			{
				CD3DX12_ROOT_PARAMETER samplerParameter;
				samplerParameter.InitAsDescriptorTable(mSamplerStateParamCount, &mDescriptorRanges[resourceCount], GetParameterVisibility());
				mRootParameters.push_back(samplerParameter);
			}
		}

		D3D12Shader::~D3D12Shader()
		{
			mByteCode.Reset();
		}

		D3D12Shader::ShaderResourceProxy::ShaderResourceProxy()
			:mConstantBuffer(nullptr), mBufferSize(0)
			, mRootResourceCount(0)
			, mCurrentResourceIndex(0), mCurrentBufferIndex(0)
			, mInit(false)
		{
			std::memset(mRootBoundResources, 0, sizeof(mRootBoundResources));
		}

		D3D12Shader::ShaderResourceProxy::~ShaderResourceProxy()
		{
			if (mConstantBuffer)
			{
				delete[] mConstantBuffer;
			}
			for (std::uint8_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				if (mRootBoundResources[i])
				{
					for (auto j = 0;j < mRootResourceCount;++j)
					{
						if (mRootBoundResources[i][j].buffers)
							delete[] mRootBoundResources[i][j].buffers;
					}
					delete[] mRootBoundResources[i];
				}
			}
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

		void D3D12Shader::ShaderResourceProxy::Init(
			std::size_t totalCount, std::size_t constantBufferCount, 
			std::size_t textureCount, std::size_t samplerStateCount)
		{
			if (mInit)
				return;
			mRootResourceCount = totalCount;
			if (mRootResourceCount == 0)
			{
				mInit = true;
				return;
			}
			for (std::uint8_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mRootBoundResources[i] = new D3D12RootBoundResource[mRootResourceCount];
				//root bound resource types are sorted in ConstantBuffer-Texture-SamplerState order.
				for (auto j = 0;j < mRootResourceCount;++j)
				{
					if (j < constantBufferCount)	//All of the constant buffers are in root parameter 0
					{
						mRootBoundResources[i][j].buffers = new D3D12ConstantBuffer[constantBufferCount];
						mRootBoundResources[i][j].count = constantBufferCount;
					}
					else if(j < textureCount)
					{
						mRootBoundResources[i][j].textures = new D3D12Texture*[textureCount];
						mRootBoundResources[i][j].count = textureCount;
					}
					else if (j < samplerStateCount)
					{
						mRootBoundResources[i][j].samplerStates = new SamplerState[samplerStateCount];
						mRootBoundResources[i][j].count = samplerStateCount;
					}
					else
					{
						assert(0 && "Wrong root resource count.");
					}
				}
			}
			mInit = true;
		}

		void D3D12Shader::ShaderResourceProxy::BeginUpdateResource(D3D12RootResourceType resourceType)
		{
			auto frameResourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			if (resourceType == D3D12RootResourceType::ConstantBuffers)
			{
				mCurrentResourceIndex = 0;
				mCurrentBufferIndex = 0;
			}
			mRootBoundResources[frameResourceIndex][mCurrentResourceIndex].type = resourceType;
		}

		void D3D12Shader::ShaderResourceProxy::EndUpdateResource()
		{

		}

		void D3D12Shader::ShaderResourceProxy::AddConstantBuffer(const D3D12ConstantBuffer& constantBuffer)
		{
			auto frameResourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto& boundResource = mRootBoundResources[frameResourceIndex];
			boundResource[mCurrentResourceIndex].buffers[mCurrentBufferIndex++] = constantBuffer;
		}

		const D3D12RootBoundResource* D3D12Shader::ShaderResourceProxy::GetRootBoundResources()
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			return mRootBoundResources[resourceIndex];
		}


		const IShaderMacros* D3D12Shader::GetMacros()const
		{
			return &mMacros;
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

		std::size_t D3D12Shader::GetParameterCount()const
		{
			return mDesc.BoundResources;
		}

		bool D3D12Shader::SetParameter(const IShaderParameter* parameter)
		{
			assert(parameter != nullptr && "parameter cannot be nullptr.");
			auto parameterType = parameter->GetType();
			if (parameterType == ShaderParameterType::UNKNOWN)
			{
				LOG_WARNING("Unknown shader parameter type when set shader {0}", mName.c_str());
				return false;
			}
			auto it = mParameters.find(parameter->GetName());
			assert(it != mParameters.end());
			std::size_t size{ 0 };
			auto parameterBuffer = parameter->Buffer(size);
			if (parameterType == ShaderParameterType::TEXTURE)
			{
				auto texture = const_cast<ITexture*>(reinterpret_cast<const ITexture*>(parameterBuffer));
				assert(texture != nullptr && "Encounter null texture!");
				texture->Commit();
				return true;
			}
			else if (parameterType == ShaderParameterType::SAMPLER)
			{
			}
			else
			{
				if (parameterBuffer)
				{
					const auto& bindingInfo = it->second;
					std::uint8_t *p = mResourceProxy->GetConstantBuffer(mTotalConstantBufferSize);
					std::uint8_t *buffer = p + mConstantBufferInfo[bindingInfo.bufferIndex].offset;
					std::memcpy(buffer + bindingInfo.offsetInBuffer, parameterBuffer, size);
					return true;
				}
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

		void D3D12Shader::GetUniformSemantics(RenderSemantics** semantics, std::uint16_t& semanticCount)
		{
			semanticCount = static_cast<std::uint16_t>(mUniformSemantics.size());
			if (semanticCount == 0)
			{
				*semantics = nullptr;
				return;
			}
			*semantics = &mUniformSemantics[0];
		}

		void D3D12Shader::UpdateRootBoundResources()
		{
			mResourceProxy->Init(GetRootParameterCount(), mConstantBufferInfo.size(), 
				mTextureParameterCount, mSamplerStateParamCount);
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto ptr = mResourceProxy->GetConstantBuffer(mTotalConstantBufferSize);
			mResourceProxy->BeginUpdateResource(D3D12RootResourceType::ConstantBuffers);
			for (std::size_t i = 0;i < mDesc.ConstantBuffers;++i)
			{
				auto bufferSize = mConstantBufferInfo[i].size;
				auto cbuffer = D3D12ConstantBufferManager::Instance()->AllocBuffer(bufferSize);
				std::memcpy(cbuffer.userMemory, ptr + mConstantBufferInfo[i].offset, bufferSize);

				mResourceProxy->AddConstantBuffer(cbuffer);
			}
			mResourceProxy->EndUpdateResource();
		}

		const Container::Vector<D3D12_ROOT_PARAMETER>& D3D12Shader::GetRootParameters()const
		{
			return mRootParameters;
		}

		std::size_t D3D12Shader::GetRootParameterCount()const
		{
			return mRootParameters.size();
		}


		const D3D12RootBoundResource* D3D12Shader::GetRootBoundResources()
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
			case ShaderType::HULL:
				return D3D12_SHADER_VISIBILITY_HULL;
			case ShaderType::DOMAIN:
				return D3D12_SHADER_VISIBILITY_DOMAIN;
			default:
				return D3D12_SHADER_VISIBILITY_ALL;
			}
		}

		void D3D12Shader::GetShaderModelString(char* buf)
		{
			std::stringstream ss;
			if (ShaderType::VERTEX == mType)
				ss << "vs_";
			else if (ShaderType::FRAGMENT == mType)
				ss << "ps_";
			else if (ShaderType::GEOMETRY == mType)
				ss << "gs_";
			else if (ShaderType::HULL == mType)
				ss << "hs_";
			else if (ShaderType::DOMAIN == mType)
				ss << "ds_";

			if (D3D_SHADER_MODEL_5_1 == mShaderModel)
				ss << "5_1";
			if (D3D_SHADER_MODEL_6_0 == mShaderModel)
				ss << "5_1";
			if (D3D_SHADER_MODEL_6_1 == mShaderModel)
				ss << "5_1";
			if (D3D_SHADER_MODEL_6_2 == mShaderModel)
				ss << "5_1";

#ifdef _MSC_VER
			strcpy_s(buf, ss.str().length() + 1, ss.str().c_str());
#else
			std::strcpy(buf, ss.str().c_str());
#endif
		}



		void D3D12Shader::CompileImpl()
		{
			D3D_SHADER_MACRO* pMacros = nullptr;
			auto macroCount = mMacros.GetMacroCount();
			if (macroCount)
			{
				pMacros = g_RenderAllocator.Allocate<D3D_SHADER_MACRO>(macroCount + 1);
				auto macroPairs = g_RenderAllocator.Allocate<MacroPair>(macroCount);
				std::memset(&pMacros[macroCount], 0, sizeof(D3D_SHADER_MACRO));
				mMacros.GetAllMacros(&macroPairs);
				auto idx = 0;
				for (auto i = 0;i < macroCount; ++i,++idx)
				{
					auto name = macroPairs[i].name;
					auto nameSize = std::strlen(name) + 1;
					pMacros[idx].Name = g_RenderAllocator.Allocate<const char>(nameSize);
#ifdef _MSC_VER
					strcpy_s(const_cast<char*>(pMacros[idx].Name), nameSize, name);
#else
					std::strcpy(const_cast<char*>(pMacros[idx].Name), name);
#endif
					auto definition = macroPairs[i].value;
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
			
			GetShaderModelString(shaderModel);
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
				LOG_ERROR("{0}", ss.str().c_str());
				throw ShaderCompileException("Failed to compile shader!");
			}
			else
			{
				LOG_INFO("Succeeded compiling shader {0}", mName);
			}
		}
	}
}