#pragma once
#include "sceneexportdef.h"
#include "camera.h"

namespace LightningGE
{
	namespace Scene
	{
		struct LIGHTNINGGE_SCENE_API SceneRenderData
		{
			Camera* camera;
		};
	}
}