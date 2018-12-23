#pragma once
#include "IScene.h"

namespace Lightning
{
	namespace Scene
	{
		struct ISceneManager
		{
			virtual INTERFACECALL ~ISceneManager() = default;
			virtual IScene* INTERFACECALL CreateScene() = 0;
			virtual IScene* INTERFACECALL GetForegroundScene() = 0;
			virtual void INTERFACECALL SetForegroundScene(IScene* scene) = 0;
			virtual void INTERFACECALL Update() = 0;
			virtual void INTERFACECALL DestroyScene(const std::uint32_t sceneId) = 0;
			virtual void INTERFACECALL DestroyAllScenes() = 0;
		};
	}
}