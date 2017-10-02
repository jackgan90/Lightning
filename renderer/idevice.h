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
			virtual void ReleaseRenderResources() = 0;
		};
		using DevicePtr = std::shared_ptr<IDevice>;
	}
}
