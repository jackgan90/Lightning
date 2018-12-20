#pragma once
#include <type_traits>
#include "ICamera.h"

namespace Lightning
{
	namespace Scene
	{
		struct SceneRenderData
		{
			ICamera* camera;
		};
		static_assert(std::is_pod<SceneRenderData>::value, "SceneRenderData is not a POD type.");
	}
}
