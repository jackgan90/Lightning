#pragma once
#include "Plugin.h"
#include "ISceneManager.h"
#include "IPrimitive.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IScenePlugin : public Plugin
		{
			virtual Scene::ISceneManager* GetSceneManager() = 0;
			virtual std::shared_ptr<Scene::IPrimitive> CreateCube(float width, float height, float thickness) = 0;
			virtual std::shared_ptr<Scene::IPrimitive> CreateCylinder(float height, float radius) = 0;
			virtual std::shared_ptr<Scene::IPrimitive> CreateHemisphere(float radius) = 0;
			virtual std::shared_ptr<Scene::IPrimitive> CreateSphere(float radius) = 0;
		};
	}
}