#pragma once
#include <memory>
#include "container.h"
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "idepthstencilbuffer.h"
#include "pipelinestate.h"
#include "igpubuffer.h"
#include "ivertexbuffer.h"
#include "iindexbuffer.h"
#include "ishader.h"
#include "rect.h"
#include "color.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::container;
		class LIGHTNING_RENDER_API IDevice
		{
		public:
			virtual ~IDevice() = default;
			//clear a specified render target,possibly parts of it defined by an array of rects
			virtual void ClearRenderTarget(const SharedRenderTargetPtr& rt, const ColorF& color, const RectIList* rects=nullptr) = 0;
			virtual void ClearDepthStencilBuffer(const SharedDepthStencilBufferPtr& buffer, DepthStencilClearFlags flags, float depth, std::uint8_t stencil, const RectIList* rects = nullptr) = 0;
			virtual SharedVertexBufferPtr CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor) = 0;
			virtual SharedIndexBufferPtr CreateIndexBuffer(std::uint32_t bufferSize, IndexType type) = 0;
			virtual SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap) = 0;
			virtual SharedShaderPtr GetDefaultShader(ShaderType type) = 0;
			virtual void ApplyRenderTargets(const container::vector<SharedRenderTargetPtr>& renderTargets, const SharedDepthStencilBufferPtr& dsBuffer) = 0;
			virtual void ApplyPipelineState(const PipelineState& state) = 0;
			//bind pBuffer to a GPU slot(does not copy data,just binding), each invocation will override previous binding
			virtual void BindGPUBuffer(std::uint8_t slot, const SharedGPUBufferPtr& buffer) = 0;
			virtual void DrawVertex(const std::size_t vertexCountPerInstance, const std::size_t instanceCount, const std::size_t firstVertexIndex, const std::size_t instanceDataOffset) = 0;
			virtual void DrawIndexed(const std::size_t indexCountPerInstance, const std::size_t instanceCount, const std::size_t firstIndex, const std::size_t indexDataOffset, const std::size_t instanceDataOffset) = 0;
			virtual void BeginFrame(const std::size_t frameResourceIndex) = 0;
			virtual void EndFrame(const std::size_t frameResourceIndex) = 0;
		};
		using SharedDevicePtr = std::shared_ptr<IDevice>;
	}
}
