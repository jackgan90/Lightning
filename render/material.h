#pragma once
#include <memory>
#include "rendererexportdef.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API Material
		{
		};
		using SharedMaterialPtr = std::shared_ptr<Material>;
	}
}