#include <d3dx12.h>
#include <d3dcompiler.h>
#include <functional>
//#include "rendererfactory.h"
#include "common.h"
#include "irenderer.h"
#include "d3d12device.h"
#include "d3d12rendertarget.h"
#include "d3d12depthstencilbuffer.h"
#include "d3d12swapchain.h"
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
			"	float4x4 wvp;"
			"};\n"
			"float4 main(float4 position:POSITION):SV_POSITION\n"
			"{\n"
				"return mul(wvp, position);\n"
			"}\n";
		const char* const DEFAULT_PS_SOURCE =
			"float4 main(void):COLOR\n"
			"{\n"
				"return float4(1.0f, 0.0f, 0.0f, 1.0f);\n"
			"}\n";
		D3D12Device::D3D12Device(const ComPtr<ID3D12Device>& pDevice, const SharedFileSystemPtr& fs)
			:Device(), m_fs(fs), m_pipelineDesc{}, m_pInputElementDesc(nullptr)
		{
			m_smallObjAllocator = std::make_unique<StackAllocator<true, 16, 8192>>();
			m_device = pDevice;
			D3D12_COMMAND_QUEUE_DESC desc = {};
			HRESULT hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create command queue!");
			}
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			for (size_t i = 0; i < config.SwapChainBufferCount; i++)
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
			m_pipelineState = pipelineState;
			m_commandList->SetPipelineState(m_pipelineState.Get());
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
			UpdatePSOInputLayout(state.vertexComponents);
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
			auto defaultShader = m_shaderMgr->CreateShaderFromSource(ShaderType::VERTEX, "[Built-in]default.vs", DEFAULT_VS_SOURCE, ShaderDefine());
			m_currentPipelineState.vs = defaultShader.get();
			VertexComponent defaultComponent{ EngineSemantics[0], 0, RenderFormat::R32G32B32_FLOAT, 0, false, 0};
			m_currentPipelineState.vertexComponents[defaultComponent] = 0;
			m_currentPipelineState.depthStencilState.depthTestEnable = false;
			ApplyPipelineState(m_currentPipelineState);
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
				m_pipelineDesc.VS.pShaderBytecode = d3d12shader->GetByteCodeBuffer();
				m_pipelineDesc.VS.BytecodeLength = d3d12shader->GetByteCodeBufferSize();
			}
		}

		void D3D12Device::UpdatePSOInputLayout(const VertexComponentBoundMap& components)
		{
			if (components.empty())
				return;
			if (m_pInputElementDesc)
				DEALLOC(m_smallObjAllocator, m_pInputElementDesc);
			m_pInputElementDesc = ALLOC_ARRAY(m_smallObjAllocator, components.size(), D3D12_INPUT_ELEMENT_DESC);
			std::size_t i = 0;
			for (auto it = components.cbegin();it != components.cend();++it)
			{
				const auto& component = it->first;
				m_pInputElementDesc[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
				m_pInputElementDesc[i].Format = D3D12TypeMapper::MapRenderFormat(component.format);
				m_pInputElementDesc[i].InputSlot = it->second;
				m_pInputElementDesc[i].InputSlotClass = component.isInstance ? \
					D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				m_pInputElementDesc[i].InstanceDataStepRate = component.instanceStepRate;
				m_pInputElementDesc[i].SemanticIndex = component.semanticIndex;
				m_pInputElementDesc[i].SemanticName = component.semanticItem.name;
				++i;
			}
			D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc = m_pipelineDesc.InputLayout;

			// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
			inputLayoutDesc.NumElements = components.size();
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

			//TODO :create root signature based on shader parameters
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			CD3DX12_ROOT_PARAMETER cbParameters[1]; 
			cbParameters[0].InitAsConstantBufferView(0);
			//TODO : deny other shader stages
			rootSignatureDesc.Init(1, cbParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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

		void D3D12Device::BeginFrame(const UINT backBufferIndex)
		{
			m_commandAllocators[backBufferIndex]->Reset();
			m_commandList->Reset(m_commandAllocators[backBufferIndex].Get(), nullptr);
		}

		void D3D12Device::ApplyRenderTargets(const RenderTargetList& renderTargets, const IDepthStencilBuffer* dsBuffer)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles = ALLOC_ARRAY(m_smallObjAllocator, renderTargets.size(), D3D12_CPU_DESCRIPTOR_HANDLE);
			for (std::size_t i = 0; i < renderTargets.size();++i)
			{
				rtvHandles[i] = static_cast<const D3D12RenderTarget*>(renderTargets[i])->GetCPUHandle();
			}
			auto dsHandle = static_cast<const D3D12DepthStencilBuffer*>(dsBuffer)->GetCPUHandle();
			m_commandList->OMSetRenderTargets(renderTargets.size(), rtvHandles, FALSE, &dsHandle);
			DEALLOC(m_smallObjAllocator, rtvHandles);
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
				//should consider dangling pointer,when this buffer is released m_bufferCommitMap still holds a pointer to
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