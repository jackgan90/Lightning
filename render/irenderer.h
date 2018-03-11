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

namespace LightningGE
{
	namespace Render
	{
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
			//set default render target clear color.At the beginning of each frame,the back buffer is cleared to this color
			virtual void SetClearColor(const ColorF& color) = 0;
			//get the current frame index
			virtual std::uint64_t GetCurrentFrameCount()const = 0;
			virtual std::uint8_t GetFrameResourceIndex()const = 0;
			virtual void AddRenderPass(RenderPassType type) = 0;
			virtual void Draw(const RenderItem& item) = 0;
		};
	}
}