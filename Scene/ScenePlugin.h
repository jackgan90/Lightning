#pragma once
#include "PluginSystem/Plugin.h"
#include "ISceneManager.h"
#include "IPrimitive.h"

namespace Lightning
{
	namespace Plugins
	{
		class ScenePlugin : public Plugin
		{
		public:
			virtual Scene::ISceneManager* GetSceneManager() = 0;
			virtual Scene::IPrimitive* CreateCube(float width, float height, float thickness) = 0;
			virtual Scene::IPrimitive* CreateCylinder(float height, float radius) = 0;
			virtual Scene::IPrimitive* CreateHemisphere(float radius) = 0;
			virtual Scene::IPrimitive* CreateSphere(float radius) = 0;
		};
	}
}