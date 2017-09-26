#pragma once
#include "rendererexportdef.h"
#include "irendertarget.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IDevice
		{
		public:
			virtual ~IDevice() {}
			virtual RenderTargetPtr CreateRenderTarget() = 0;
		};
		using DevicePtr = std::shared_ptr<IDevice>;
	}
}
