#include <d3dx12.h>
#include <d3dcompiler.h>
#include <functional>
//#include "rendererfactory.h"
#include "common.h"
#include "irenderer.h"
#include "d3d12device.h"
#include "d3d12rendertarget.h"
#include "d3d12depthstencilbuffer.h"
#include "d3d12shader.h"
#include "d3d12typemapper.h"
#include "renderconstants.h"
#include "semantics.h"
#include "shadermanager.h"
#include "logger.h"
#include "configmanager.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::LogLevel;
		using Foundation::logger;
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using Foundation::StackAllocator;
		const char* const DEFAULT_VS_SOURCE =
			"cbuffer VS_IN : register(b0)\n"
			"{\n"
			"	float4x4 wvp;\n"
			"};\n"
			"float4 main(float4 position:POSITION):SV_POSITION\n"
			"{\n"
				"return mul(wvp, position);\n"
			"}\n";
		const char* const DEFAULT_PS_SOURCE =
			"float4 main(void):SV_Target\n"
			"{\n"
				"return float4(1.0f, 0.0f, 0.0f, 1.0f);\n"
			"}\n";
		D3D12Device::D3D12Device(const ComPtr<ID3D12Device>& pDevice, const SharedFileSystemPtr& fs)
			:Device(), m_fs(fs), m_pipelineDesc{}, m_pInputElementDesc(nullptr), m_currentDSBuffer(nullptr)
		{
			m_smallObjAllocator = std::make_unique<StackAllocator<true, 16, 8192>>();
			m_device = pDevice;
			D3D12_COMMAND_QUEUE_DESC desc = {};
			HRESULT hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create command queue!");
			}
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				ComPtr<ID3D12CommandAllocator> allocator;
				hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
				if (FAILED(hr))
				{
					throw DeviceInitException("Failed to create command allocator!");
				}
				m_commandAllocators.push_back(allocator);
			}
			hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create command list!");
			}
			m_shaderMgr = std::make_unique<D3D12ShaderManager>(this, fs);
			m_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			//should create first default pipeline state
			SetUpDefaultPipelineStates();
			m_commandList->Close();
			//m_shaderMgr->CreateShaderFromFile(ShaderType::VERTEX, "default.vs", ShaderDefine());
		}

		D3D12Device::~D3D12Device()
		{
			if (m_pInputElementDesc)
			{
				DEALLOC(m_smallObjAllocator, m_pInputElementDesc);
			}
			for (auto it = m_bufferCommitMap.begin();it != m_bufferCommitMap.end();++it)
			{
				it->second.uploadHeap.Reset();
				it->second.defaultHeap.Reset();
			}
			m_bufferCommitMap.clear();
			m_shaderMgr.reset();
		}

		void D3D12Device::ClearRenderTarget(IRenderTarget* rt, const ColorF& color, const RectIList* rects)
		{
			D3D12RenderTarget *pTarget = static_cast<D3D12RenderTarget*>(rt);
			ComPtr<ID3D12Resource> nativeRenderTarget = pTarget->GetNative();
			//should check the type of the rt to transit it from previous state to render target state
			//currently just check back buffer render target
			if (rt->IsSwapChainRenderTarget())
			{
				m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRenderTarget.Get(),
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
			}

			//TODO : check gpu?
			const float clearColor[] = { color.r(), color.g(), color.b(), color.a() };
			auto rtvHandle = pTarget->GetCPUHandle();
			if (rects && !rects->empty())
			{
				D3D12_RECT* d3dRect = ALLOC_ARRAY(m_smallObjAllocator, rects->size(), D3D12_RECT);
				for (size_t i = 0; i < rects->size(); i++)
				{
					d3dRect[i].left = (*rects)[i].left();
					d3dRect[i].right = (*rects)[i].right();
					d3dRect[i].top = (*rects)[i].top();
					d3dRect[i].bottom = (*rects)[i].bottom();
				}
				m_commandList->ClearRenderTargetView(rtvHandle, clearColor, rects->size(), d3dRect);
				DEALLOC(m_smallObjAllocator, d3dRect);
			}
			else
			{
				m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
			}
		}


		SharedVertexBufferPtr D3D12Device::CreateVertexBuffer()
		{
			//TODO : replace with d3d12 vertex buffer
			return SharedVertexBufferPtr();
		}

		void D3D12Device::ApplyRasterizerState(const RasterizerState& state)
		{
			Device::ApplyRasterizerState(state);
			D3D12_RASTERIZER_DESC* pDesc = &m_pipelineDesc.RasterizerState;
			pDesc->FillMode = D3D12TypeMapper::MapFillMode(state.fillMode);
			pDesc->CullMode = D3D12TypeMapper::MapCullMode(state.cullMode);
			pDesc->FrontCounterClockwise = state.frontFaceWindingOrder == FrontFaceWindingOrder::COUNTER_CLOCKWISE;
		}

		void D3D12Device::ApplyBlendState(const BlendState& state)
		{
			Device::ApplyBlendState(state);
			auto boundIndex = state.renderTarget ? state.renderTarget->GetBoundIndex() : 0;
			assert(boundIndex >= 0);
			D3D12_RENDER_TARGET_BLEND_DESC* pDesc = &m_pipelineDesc.BlendState.RenderTarget[boundIndex];
			pDesc->BlendEnable = state.enable;
			pDesc->BlendOp = D3D12TypeMapper::MapBlendOp(state.colorOp);
			pDesc->BlendOpAlpha = D3D12TypeMapper::MapBlendOp(state.alphaOp);
			pDesc->SrcBlend = D3D12TypeMapper::MapBlendFactor(state.srcColorFactor);
			pDesc->SrcBlendAlpha = D3D12TypeMapper::MapBlendFactor(state.srcAlphaFactor);
			pDesc->DestBlend = D3D12TypeMapper::MapBlendFactor(state.destColorFactor);
			pDesc->DestBlendAlpha = D3D12TypeMapper::MapBlendFactor(state.destAlphaFactor);
		}

		void D3D12Device::ApplyDepthStencilState(const DepthStencilState& state)
		{
			Device::ApplyDepthStencilState(state);
			D3D12_DEPTH_STENCIL_DESC* pDesc = &m_pipelineDesc.DepthStencilState;
			pDesc->DepthEnable = state.depthTestEnable;
			pDesc->DepthFunc = D3D12TypeMapper::MapCmpFunc(state.depthCmpFunc);
			pDesc->DepthWriteMask = state.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
			pDesc->StencilEnable = state.stencilEnable;
			pDesc->StencilReadMask = state.stencilReadMask;
			pDesc->StencilWriteMask = state.stencilWriteMask;

			pDesc->FrontFace.StencilFunc = D3D12TypeMapper::MapCmpFunc(state.frontFace.cmpFunc);
			pDesc->FrontFace.StencilPassOp = D3D12TypeMapper::MapStencilOp(state.frontFace.passOp);
			pDesc->FrontFace.StencilFailOp = D3D12TypeMapper::MapStencilOp(state.frontFace.failOp);
			pDesc->FrontFace.StencilDepthFailOp = D3D12TypeMapper::MapStencilOp(state.frontFace.depthFailOp);

			pDesc->BackFace.StencilFunc = D3D12TypeMapper::MapCmpFunc(state.backFace.cmpFunc);
			pDesc->BackFace.StencilPassOp = D3D12TypeMapper::MapStencilOp(state.backFace.passOp);
			pDesc->BackFace.StencilFailOp = D3D12TypeMapper::MapStencilOp(state.backFace.failOp);
			pDesc->BackFace.StencilDepthFailOp = D3D12TypeMapper::MapStencilOp(state.backFace.depthFailOp);
			if (state.depthTestEnable || state.stencilEnable)
			{
				if (m_currentDSBuffer)
				{
					m_pipelineDesc.DSVFormat = D3D12TypeMapper::MapRenderFormat(m_currentDSBuffer->GetRenderFormat());
				}
			}

		}

		void D3D12Device::ApplyPipelineState(const PipelineState& state)
		{
			Device::ApplyPipelineState(state);
			ComPtr<ID3D12PipelineState> pipelineState;
			auto hashValue = std::hash<PipelineState>{}(state);
			auto cachedStateObject = m_pipelineCache.find(hashValue);
			if (cachedStateObject != m_pipelineCache.end())
			{
				pipelineState = cachedStateObject->second;
			}
			else
			{
				pipelineState = CreateAndCachePipelineState(state, hashValue);
			}
			m_d3d12PipelineState = pipelineState;
			m_commandList->SetPipelineState(m_d3d12PipelineState.Get());
			if (m_pipelineDesc.pRootSignature)
			{
				m_commandList->SetGraphicsRootSignature(m_pipelineDesc.pRootSignature);
				BindAllShaderResources();
			}
		}

		void D3D12Device::BindAllShaderResources()
		{
			std::size_t rootParameterIndex{ 0 };
			if (m_devicePipelineState.vs)
			{
				BindShaderResources(m_devicePipelineState.vs, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(m_devicePipelineState.vs)->GetRootParameterCount();
			}
			if (m_devicePipelineState.fs)
			{
				BindShaderResources(m_devicePipelineState.fs, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(m_devicePipelineState.fs)->GetRootParameterCount();
			}
			if (m_devicePipelineState.gs)
			{
				BindShaderResources(m_devicePipelineState.gs, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(m_devicePipelineState.gs)->GetRootParameterCount();
			}
			if (m_devicePipelineState.hs)
			{
				BindShaderResources(m_devicePipelineState.hs, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(m_devicePipelineState.hs)->GetRootParameterCount();
			}
			if (m_devicePipelineState.ds)
			{
				BindShaderResources(m_devicePipelineState.ds, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(m_devicePipelineState.ds)->GetRootParameterCount();
			}
		}

		void D3D12Device::BindShaderResources(IShader* pShader, UINT rootParameterIndex)
		{
			auto pD3D12Shader = static_cast<D3D12Shader*>(pShader);
			auto const& boundResources = pD3D12Shader->GetRootBoundResources();
			for (std::size_t i = 0;i < boundResources.size();++i)
			{
				const auto& boundResource = boundResources[i];
				switch (boundResource.type)
				{
				case D3D12RootBoundResourceType::DescriptorTable:
					//TODO : should bind all descriptor heaps not only the heap that this shader uses
					m_commandList->SetDescriptorHeaps(1, &boundResource.descriptorTableHeap);
					m_commandList->SetGraphicsRootDescriptorTable(rootParameterIndex + i, boundResource.descriptorTableHandle);
					break;
				case D3D12RootBoundResourceType::ConstantBufferView:
					m_commandList->SetGraphicsRootConstantBufferView(rootParameterIndex + i, boundResource.GPUVirtualAddress);
					break;
				case D3D12RootBoundResourceType::ShaderResourceView:
					m_commandList->SetGraphicsRootShaderResourceView(rootParameterIndex + i, boundResource.GPUVirtualAddress);
					break;
				case D3D12RootBoundResourceType::UnorderedAccessView:
					m_commandList->SetGraphicsRootUnorderedAccessView(rootParameterIndex + i, boundResource.GPUVirtualAddress);
					break;
				case D3D12RootBoundResourceType::Constant:
					m_commandList->SetGraphicsRoot32BitConstants(rootParameterIndex + i, boundResource.constant32BitValue.num32BitValues, boundResource.constant32BitValue.p32BitValues, boundResource.constant32BitValue.dest32BitValueOffset);
					break;
				default:
					break;
				}
			}
		}

		void D3D12Device::ApplyViewports(const RectFList& vp)
		{

		}

		void D3D12Device::ApplyScissorRects(const RectFList& scissorRects)
		{

		}

		ComPtr<ID3D12PipelineState> D3D12Device::CreateAndCachePipelineState(const PipelineState& state, std::size_t hashValue)
		{
			ComPtr<ID3D12PipelineState> pipelineState;
			ApplyRasterizerState(state.rasterizerState);
			ApplyBlendState(state.blendState);
			ApplyDepthStencilState(state.depthStencilState);
			std::vector<IShader*> shaders;
			ApplyShader(state.vs);
			ApplyShader(state.fs);
			ApplyShader(state.gs);
			ApplyShader(state.hs);
			ApplyShader(state.ds);
			if (state.vs)
				shaders.push_back(state.vs);
			if (state.fs)
				shaders.push_back(state.fs);
			if (state.gs)
				shaders.push_back(state.gs);
			if (state.hs)
				shaders.push_back(state.hs);
			if (state.ds)
				shaders.push_back(state.ds);
			UpdatePSOInputLayout(state.inputLayouts);
			auto rootSignature = GetRootSignature(shaders);
			m_pipelineDesc.pRootSignature = rootSignature.Get();
			auto hr = m_device->CreateGraphicsPipelineState(&m_pipelineDesc, IID_PPV_ARGS(&pipelineState));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to apply pipeline state!");
				return pipelineState;
			} 
			m_pipelineCache.emplace(hashValue, pipelineState);
			return pipelineState;
		}

		void D3D12Device::SetUpDefaultPipelineStates()
		{
			m_defaultShaders[ShaderType::VERTEX] = m_shaderMgr->CreateShaderFromSource(ShaderType::VERTEX, "[Built-in]default.vs", DEFAULT_VS_SOURCE, ShaderDefine());
			m_defaultShaders[ShaderType::FRAGMENT] = m_shaderMgr->CreateShaderFromSource(ShaderType::FRAGMENT, "[Built-in]default.ps", DEFAULT_PS_SOURCE, ShaderDefine()); 
			m_devicePipelineState.vs = m_defaultShaders[ShaderType::VERTEX].get();
			VertexComponent defaultComponent{ EngineSemantics[0], 0, RenderFormat::R32G32B32_FLOAT, 0, false, 0};
			VertexInputLayout layout;
			layout.slot = 0;
			layout.components.push_back(defaultComponent);
			m_devicePipelineState.inputLayouts.push_back(layout);
			m_devicePipelineState.depthStencilState.depthTestEnable = false;
			ApplyPipelineState(m_devicePipelineState);
		}

		SharedShaderPtr D3D12Device::CreateShader(ShaderType type, const std::string& shaderName, 
			const char* const shaderSource, const ShaderDefine& defineMap)
		{
			return std::make_shared<D3D12Shader>(m_device.Get(), type, shaderName, DEFAULT_SHADER_ENTRY, shaderSource);
		}

		void D3D12Device::ApplyShader(IShader* pShader)
		{
			if (pShader)
			{
				auto d3d12shader = static_cast<D3D12Shader*>(pShader);
				auto byteCode = d3d12shader->GetByteCodeBuffer();
				auto byteCodeLength = d3d12shader->GetByteCodeBufferSize();
				switch (pShader->GetType())
				{
				case ShaderType::VERTEX:
					m_pipelineDesc.VS.pShaderBytecode = byteCode;
					m_pipelineDesc.VS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::FRAGMENT:
					m_pipelineDesc.PS.pShaderBytecode = byteCode;
					m_pipelineDesc.PS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::GEOMETRY:
					m_pipelineDesc.GS.pShaderBytecode = byteCode;
					m_pipelineDesc.GS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::TESSELATION_CONTROL:
					m_pipelineDesc.HS.pShaderBytecode = byteCode;
					m_pipelineDesc.HS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::TESSELATION_EVALUATION:
					m_pipelineDesc.DS.pShaderBytecode = byteCode;
					m_pipelineDesc.DS.BytecodeLength = byteCodeLength;
					break;
				default:
					break;
				}
			}
		}

		void D3D12Device::UpdatePSOInputLayout(const std::vector<VertexInputLayout>& inputLayouts)
		{
			if (inputLayouts.empty())
				return;
			if (m_pInputElementDesc)
				DEALLOC(m_smallObjAllocator, m_pInputElementDesc);
			std::size_t inputElementCount{ 0 };
			for (const auto& inputLayout : inputLayouts)
			{
				inputElementCount += inputLayout.components.size();
			}
			m_pInputElementDesc = ALLOC_ARRAY(m_smallObjAllocator, inputElementCount, D3D12_INPUT_ELEMENT_DESC);
			std::size_t i = 0;
			for (const auto& inputLayout : inputLayouts)
			{
				for (const auto& component : inputLayout.components)
				{
					m_pInputElementDesc[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
					m_pInputElementDesc[i].Format = D3D12TypeMapper::MapRenderFormat(component.format);
					m_pInputElementDesc[i].InputSlot = inputLayout.slot;
					m_pInputElementDesc[i].InputSlotClass = component.isInstance ? \
						D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
					m_pInputElementDesc[i].InstanceDataStepRate = component.isInstance ? component.instanceStepRate : 0;
					m_pInputElementDesc[i].SemanticIndex = component.semanticIndex;
					m_pInputElementDesc[i].SemanticName = component.semanticItem.name;
					++i;
				}
			}
			D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc = m_pipelineDesc.InputLayout;

			// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
			inputLayoutDesc.NumElements = inputElementCount;
			inputLayoutDesc.pInputElementDescs = m_pInputElementDesc;
		}

		ComPtr<ID3D12RootSignature> D3D12Device::GetRootSignature(const std::vector<IShader*>& shaders)
		{	
			size_t seed = 0;
			boost::hash_combine(seed, shaders.size());
			for (const auto& s : shaders)
			{
				boost::hash_combine(seed, s->GetHashValue());
			}
			auto it = m_rootSignatures.find(seed);
			if (it != m_rootSignatures.end())
				return it->second;

			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			std::vector<D3D12_ROOT_PARAMETER> cbParameters;
			for (std::size_t i = 0;i < shaders.size();++i)
			{
				const auto& parameters = static_cast<D3D12Shader*>(shaders[i])->GetRootParameters();
				cbParameters.insert(cbParameters.end(), parameters.begin(), parameters.end());
			}
			rootSignatureDesc.Init(cbParameters.size(), &cbParameters[0], 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			auto hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
			if (FAILED(hr))
			{
				return ComPtr<ID3D12RootSignature>();
			}

			ComPtr<ID3D12RootSignature> rootSignature;
			hr = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
			if (FAILED(hr))
			{
				return ComPtr<ID3D12RootSignature>();
			}
			m_rootSignatures[seed] = rootSignature;
			return rootSignature;
		}

		void D3D12Device::BeginFrame(const UINT frameResourceIndex)
		{
			m_commandAllocators[frameResourceIndex]->Reset();
			m_commandList->Reset(m_commandAllocators[frameResourceIndex].Get(), nullptr);
			m_frameResourceIndex = frameResourceIndex;
		}

		void D3D12Device::ApplyRenderTargets(const RenderTargetList& renderTargets, const IDepthStencilBuffer* dsBuffer)
		{
			//TODO : actually should set pipeline description based on PipelineState rather than set them here
			auto rtvHandles = m_frameRTVHandles[m_frameResourceIndex];
			for (std::size_t i = 0; i < renderTargets.size();++i)
			{
				rtvHandles[i] = static_cast<const D3D12RenderTarget*>(renderTargets[i])->GetCPUHandle();
				if (i == 0)
				{
					m_pipelineDesc.SampleDesc.Count = renderTargets[i]->GetSampleCount();
					m_pipelineDesc.SampleDesc.Quality = renderTargets[i]->GetSampleQuality();
				}
				m_pipelineDesc.RTVFormats[i] = D3D12TypeMapper::MapRenderFormat(renderTargets[i]->GetRenderFormat());
			}
			auto dsHandle = static_cast<const D3D12DepthStencilBuffer*>(dsBuffer)->GetCPUHandle();
			m_commandList->OMSetRenderTargets(renderTargets.size(), rtvHandles, FALSE, &dsHandle);
			m_currentDSBuffer = dsBuffer;
			m_pipelineDesc.NumRenderTargets = renderTargets.size();
		}

		void D3D12Device::CommitGPUBuffer(const GPUBuffer* pBuffer)
		{
			auto it = m_bufferCommitMap.find(pBuffer);
			auto bufferSize = pBuffer->GetBufferSize();
			GPUBufferCommit bufferCommit;
			if (it == m_bufferCommitMap.end())
			{
				bufferCommit.type = pBuffer->GetType();
				m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
							D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
							D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&bufferCommit.defaultHeap));
				m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
							D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
							D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferCommit.uploadHeap));
				//TODO : should consider dangling pointer,when this buffer is released m_bufferCommitMap still holds a pointer to
				//the GPUBuffer,but since the buffer is destroyed,there should be no other reference to it so leave it
				//in m_bufferCommitMap wouldn't cause problems,but still, need to find a good way to resolve such condition
				switch (bufferCommit.type)
				{
				case GPUBufferType::VERTEX:
					bufferCommit.vertexBufferView.BufferLocation = bufferCommit.defaultHeap->GetGPUVirtualAddress();
					bufferCommit.vertexBufferView.SizeInBytes = bufferSize;
					bufferCommit.vertexBufferView.StrideInBytes = static_cast<VertexBuffer*>(const_cast<GPUBuffer*>(pBuffer))->GetVertexSize();
					break;
				case GPUBufferType::INDEX:
					bufferCommit.indexBufferView.BufferLocation = bufferCommit.defaultHeap->GetGPUVirtualAddress();
					bufferCommit.indexBufferView.SizeInBytes = bufferSize;
					bufferCommit.indexBufferView.Format = D3D12TypeMapper::MapIndexType(static_cast<const IndexBuffer*>(pBuffer)->GetIndexType());
					break;
				default:
					logger.Log(LogLevel::Warning, "Unknown GPUBuffer Commit!");
					break;
				}
				m_bufferCommitMap[pBuffer] = bufferCommit;
			}
			else
			{
				bufferCommit = it->second;
				m_commandList->ResourceBarrier(1, 
					&CD3DX12_RESOURCE_BARRIER::Transition(bufferCommit.defaultHeap.Get(), 
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST));
			}
			D3D12_SUBRESOURCE_DATA subResourceData{};
			subResourceData.pData = pBuffer->GetBuffer();
			subResourceData.RowPitch = bufferSize;
			subResourceData.SlicePitch = bufferSize;
			UpdateSubresources(m_commandList.Get(), bufferCommit.defaultHeap.Get(), bufferCommit.uploadHeap.Get(), 0, 0, 1, &subResourceData);
			m_commandList->ResourceBarrier(1, 
				&CD3DX12_RESOURCE_BARRIER::Transition(bufferCommit.defaultHeap.Get(), 
					D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
		}
	}
}