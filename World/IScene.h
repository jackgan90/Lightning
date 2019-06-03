#pragma once
#include <cstdint>
#include "ISpaceCamera.h"
#include "ISpaceObject.h"

namespace Lightning
{
	namespace World
	{
		using Render::ICamera;
		struct IScene : virtual ISpaceObject
		{
			virtual void Tick() = 0;
			virtual ISpaceCamera* GetActiveCamera() = 0;
			virtual ISpaceCamera* CreateCamera() = 0;
		};
	}
}