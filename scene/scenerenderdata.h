#pragma once
#include "sceneexportdef.h"
#include "camera.h"

namespace Lightning
{
	namespace Scene
	{
		struct LIGHTNING_SCENE_API SceneRenderData
		{
			Camera* camera;
		};
	}
}
