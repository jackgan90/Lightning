#pragma once
#include <memory>
#include <vector>
#include "Eigen/StdVector"
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "idepthstencilbuffer.h"
#include "pipelinestate.h"
#include "gpubuffer.h"
#include "vertexbuffer.h"
#include "ishader.h"
#include "rect.h"
#include "color.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API IDevice
		{
		public:
			virtual ~IDevice() = default;
			//clear a specified render target,possibly parts of it defined by an array of rects
			virtual void ClearRenderTarget(IRenderTarget* rt, const ColorF& color, const RectIList* rects=nullptr) = 0;
			virtual SharedVertexBufferPtr CreateVertexBuffer() = 0;
			virtual SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap) = 0;
			virtual SharedShaderPtr GetDefaultShader(ShaderType type) = 0;
			virtual void ApplyPipelineState(const PipelineState& state) = 0;
			//copy all data pointed to by pBuffer to GPU
			virtual void CommitGPUBuffer(const GPUBuffer* pBuffer) = 0;
			//bind pBuffer to a GPU slot(does not copy data,just binding), each invocation will override previous binding
			virtual void BindGPUBuffers(std::uint8_t startSlot, const std::vector<GPUBuffer*>& buffers) = 0;
			virtual void DrawVertex(const std::size_t vertexCountPerInstance, const std::size_t instanceCount, const std::size_t firstVertexIndex, const std::size_t instanceDataOffset) = 0;
			virtual void DrawIndexed(const std::size_t indexCountPerInstance, const std::size_t instanceCount, const std::size_t firstIndex, const std::size_t indexDataOffset, const std::size_t instanceDataOffset) = 0;
		};
		using SharedDevicePtr = std::shared_ptr<IDevice>;
	}
}
