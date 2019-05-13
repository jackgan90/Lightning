#pragma once
#include <memory>
#include <functional>
#include "IDevice.h"
#include "ISwapChain.h"
#include "IDepthStencilBuffer.h"
#include "Color.h"
#include "Math/Matrix.h"
#include "IDrawCommand.h"
#include "IDrawable.h"
#include "ICamera.h"
#include "IWindow.h"

namespace Lightning
{
	namespace Render
	{
		enum class DrawType
		{
			Vertex,
			Index
		};

		struct DrawParam
		{
			DrawType drawType;
			union
			{
				std::size_t vertexCount;
				std::size_t indexCount;
			};
			std::size_t instanceCount;
			union
			{
				std::size_t firstVertex;
				std::size_t firstIndex;
			};
			//A value added to each index before reading a vertex from the vertex buffer.
			//only valid for index draw type.Vertex draw type will simply ignore this value
			std::size_t baseIndex;
			std::size_t baseInstance;
		};
		static_assert(std::is_pod<DrawParam>::value, "DrawParam is not a POD type.");

		enum class RendererEvent
		{
			FRAME_BEGIN,
			FRAME_UPDATE,
			FRAME_POST_UPDATE,
			FRAME_END
		};

		struct IRenderer
		{
			virtual ~IRenderer() = default;
			//entry point of render system
			virtual void Render() = 0;
			//return the device of render system
			virtual IDevice* GetDevice() = 0;
			virtual ISwapChain* GetSwapChain() = 0;
			virtual Window::IWindow* GetOutputWindow() = 0;
			//get the current frame index
			virtual std::uint64_t GetCurrentFrameCount()const = 0;
			virtual std::size_t GetFrameResourceIndex()const = 0;
			//clear a specified render target,possibly parts of it defined by an array of rects
			virtual void ClearRenderTarget(IRenderTarget* renderTarget, const ColorF& color, 
				const RectI* rects=nullptr, std::size_t rectCount = 0) = 0;
			virtual void ClearDepthStencilBuffer(IDepthStencilBuffer* buffer, DepthStencilClearFlags flags, float depth, std::uint8_t stencil, 
				const RectI* rects = nullptr, std::size_t rectCount = 0) = 0;
			virtual void ApplyRenderTargets(const IRenderTarget*const * renderTargets, std::size_t renderTargetCount, IDepthStencilBuffer* dsBuffer) = 0;
			virtual void ApplyPipelineState(const PipelineState& state) = 0;
			virtual void ApplyViewports(const Viewport* viewports, std::size_t viewportCount) = 0;
			virtual void ApplyScissorRects(const ScissorRect* scissorRects, std::size_t scissorRectCount) = 0;
			//bind pBuffer to a GPU slot(does not copy data,just binding), each invocation will override previous binding
			virtual void BindVertexBuffer(std::size_t slot, IVertexBuffer* buffer) = 0;
			virtual void BindIndexBuffer(IIndexBuffer* buffer) = 0;
			//Adds a drawable to draw queue,thread safe.
			virtual void Draw(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera) = 0;
			//issue underlying draw call
			virtual void Draw(const DrawParam& param) = 0;
			//get near plane value corresponding to normalized device coordinate
			//different render API may have different near plane definition
			//for example OpenGL clips coordinates to [-1, 1] and DirectX clips coordinates to [0, 1]
			//This method returns 0 for DirectX and -1 for OpenGL
			virtual float GetNDCNearPlane()const = 0;
			//start the renderer
			virtual void Start() = 0;
			//Shut down the renderer
			virtual void ShutDown() = 0;
			//get default depth stencil buffer
			virtual std::shared_ptr<IDepthStencilBuffer> GetDefaultDepthStencilBuffer() = 0;
			//get default render target
			virtual std::shared_ptr<IRenderTarget> GetDefaultRenderTarget() = 0;
			//Gets the semantic corresponds to a string representing a uniform name
			virtual RenderSemantics GetUniformSemantic(const char* uniform_name) = 0;
			//Gets uniform name by RenderSemantics
			virtual const char* GetUniformName(RenderSemantics semantic) = 0;
			virtual void GetSemanticInfo(RenderSemantics semantic, SemanticIndex& index, std::string& name) = 0;
		};
	}
}
