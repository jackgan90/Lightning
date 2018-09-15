#pragma once
#include <memory>
#include <exception>
#include "renderexception.h"
#include "rendererexportdef.h"
#include "idevice.h"
#include "iswapchain.h"
#include "irendertargetmanager.h"
#include "color.h"
#include "matrix.h"
#include "renderitem.h"
#include "renderpass.h"
#include "iwindow.h"

namespace LightningGE
{
	namespace Render
	{
		using WindowSystem::IWindow;
		class LIGHTNINGGE_RENDER_API DeviceInitException : public RendererException
		{
		public:
			DeviceInitException(const char*const w):RendererException(w){}
		};

		class LIGHTNINGGE_RENDER_API SwapChainInitException : public RendererException
		{
		public:
			SwapChainInitException(const char*const w):RendererException(w){}
		};

		class LIGHTNINGGE_RENDER_API IRenderer
		{
		public:
			virtual ~IRenderer() = default;
			//entry point of render system
			virtual void Render() = 0;
			//return the device of render system
			virtual IDevice* GetDevice() = 0;
			virtual ISwapChain* GetSwapChain() = 0;
			virtual IWindow* GetOutputWindow() = 0;
			//set default render target clear color.At the beginning of each frame,the back buffer is cleared to this color
			virtual void SetClearColor(const ColorF& color) = 0;
			//get the current frame index
			virtual std::uint64_t GetCurrentFrameCount()const = 0;
			virtual std::size_t GetFrameResourceIndex()const = 0;
			virtual void AddRenderPass(RenderPassType type) = 0;
			virtual void Draw(const RenderItem& item) = 0;
			//get near plane value corresponding to normalized device coordinate
			//different render API may have different near plane definition
			//for example OpenGL clips coordinates to [-1, 1] and DirectX clips coordinates to [0, 1]
			//This method returns 0 for DirectX and -1 for OpenGL
			virtual float GetNDCNearPlane()const = 0;
			//start the renderer
			virtual void Start() = 0;
			//Shut down the renderer
			virtual void ShutDown() = 0;
		};
	}
}