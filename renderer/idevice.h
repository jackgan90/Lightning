#pragma once
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "irenderresourceuser.h"
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IDevice : public IRenderResourceUser
		{
		};
		using DevicePtr = std::shared_ptr<IDevice>;
	}
}
