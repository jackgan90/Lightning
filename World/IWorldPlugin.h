#pragma once
#include "Plugin.h"
#include "ISceneManager.h"
#include "IPrimitive.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IWorldPlugin : public Plugin
		{
			virtual World::ISceneManager* GetSceneManager() = 0;
			virtual std::shared_ptr<World::IPrimitive> CreateCube(float width, float height, float thickness) = 0;
			virtual std::shared_ptr<World::IPrimitive> CreateCylinder(float height, float radius) = 0;
			virtual std::shared_ptr<World::IPrimitive> CreateHemisphere(float radius) = 0;
			virtual std::shared_ptr<World::IPrimitive> CreateSphere(float radius) = 0;
		};
	}
}