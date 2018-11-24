#pragma once
#include "IScene.h"

namespace Lightning
{
	namespace Scene
	{
		class ISceneManager
		{
		public:
			virtual ~ISceneManager() = default;
			virtual IScene* CreateScene() = 0;
			virtual IScene* GetForegroundScene() = 0;
			virtual void SetForegroundScene(IScene* scene) = 0;
			virtual void Update() = 0;
			virtual void DestroyScene(const std::uint32_t sceneId) = 0;
			virtual void DestroyAllScenes() = 0;
		};
	}
}