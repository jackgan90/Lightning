#pragma once
#include <memory>
#include <exception>
#include <functional>
#include "RenderException.h"
#include "IDevice.h"
#include "ISwapChain.h"
#include "IDepthStencilBuffer.h"
#include "Color.h"
#include "Math/Matrix.h"
#include "IRenderUnit.h"
#include "RenderPass.h"
#include "IWindow.h"
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		class DeviceInitException : public RendererException
		{
		public:
			DeviceInitException(const char*const w):RendererException(w){}
		};

		class SwapChainInitException : public RendererException
		{
		public:
			SwapChainInitException(const char*const w):RendererException(w){}
		};

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
			virtual INTERFACECALL ~IRenderer() = default;
			//entry point of render system
			virtual void INTERFACECALL Render() = 0;
			//return the device of render system
			virtual IDevice* INTERFACECALL GetDevice() = 0;
			virtual ISwapChain* INTERFACECALL GetSwapChain() = 0;
			virtual Window::IWindow* INTERFACECALL GetOutputWindow() = 0;
			//set default render target clear color.At the beginning of each frame,the back buffer is cleared to this color
			virtual void INTERFACECALL SetClearColor(float r, float g, float b, float a) = 0;
			//get the current frame index
			virtual std::uint64_t INTERFACECALL GetCurrentFrameCount()const = 0;
			virtual std::size_t INTERFACECALL GetFrameResourceIndex()const = 0;
			virtual void INTERFACECALL AddRenderPass(RenderPassType type) = 0;
			virtual IRenderUnit* INTERFACECALL CreateRenderUnit() = 0;
			//Commit a render unit for rendering
			virtual void INTERFACECALL CommitRenderUnit(const IRenderUnit* unit) = 0;
			//clear a specified render target,possibly parts of it defined by an array of rects
			virtual void INTERFACECALL ClearRenderTarget(IRenderTarget* renderTarget, const ColorF& color, 
				const RectI* rects=nullptr, std::size_t rectCount = 0) = 0;
			virtual void INTERFACECALL ClearDepthStencilBuffer(IDepthStencilBuffer* buffer, DepthStencilClearFlags flags, float depth, std::uint8_t stencil, 
				const RectI* rects = nullptr, std::size_t rectCount = 0) = 0;
			virtual void INTERFACECALL ApplyRenderTargets(const IRenderTarget*const * renderTargets, std::size_t renderTargetCount, IDepthStencilBuffer* dsBuffer) = 0;
			virtual void INTERFACECALL ApplyPipelineState(const PipelineState& state) = 0;
			virtual void INTERFACECALL ApplyViewports(const Viewport* viewports, std::size_t viewportCount) = 0;
			virtual void INTERFACECALL ApplyScissorRects(const ScissorRect* scissorRects, std::size_t scissorRectCount) = 0;
			//bind pBuffer to a GPU slot(does not copy data,just binding), each invocation will override previous binding
			virtual void INTERFACECALL BindGPUBuffer(std::size_t slot, IGPUBuffer* buffer) = 0;
			//issue underlying draw call
			virtual void INTERFACECALL Draw(const DrawParam& param) = 0;
			//get near plane value corresponding to normalized device coordinate
			//different render API may have different near plane definition
			//for example OpenGL clips coordinates to [-1, 1] and DirectX clips coordinates to [0, 1]
			//This method returns 0 for DirectX and -1 for OpenGL
			virtual float INTERFACECALL GetNDCNearPlane()const = 0;
			//start the renderer
			virtual void INTERFACECALL Start() = 0;
			//Shut down the renderer
			virtual void INTERFACECALL ShutDown() = 0;
			//get default depth stencil buffer
			virtual IDepthStencilBuffer* INTERFACECALL GetDefaultDepthStencilBuffer() = 0;
			//Gets the semantic corresponds to a string representing a uniform name
			virtual RenderSemantics INTERFACECALL GetUniformSemantic(const char* uniform_name) = 0;
			//Gets uniform name by RenderSemantics
			virtual const char* INTERFACECALL GetUniformName(RenderSemantics semantic) = 0;
			virtual void GetSemanticInfo(RenderSemantics semantic, SemanticIndex& index, std::string& name) = 0;
		};
	}
}
