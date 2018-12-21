#pragma once
#include "IPlugin.h"
#include "ISceneManager.h"
#include "IPrimitive.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IScenePlugin : public IPlugin
		{
			virtual Scene::ISceneManager* INTERFACECALL GetSceneManager() = 0;
			virtual Scene::IPrimitive* INTERFACECALL CreateCube(float width, float height, float thickness) = 0;
			virtual Scene::IPrimitive* INTERFACECALL CreateCylinder(float height, float radius) = 0;
			virtual Scene::IPrimitive* INTERFACECALL CreateHemisphere(float radius) = 0;
			virtual Scene::IPrimitive* INTERFACECALL CreateSphere(float radius) = 0;
		};
	}
}