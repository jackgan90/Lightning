#pragma once
#include "ICamera.h"
#include "ISpaceObject.h"

namespace Lightning
{
	namespace World
	{
		struct ISpaceCamera : Render::ICamera, virtual ISpaceObject
		{

		};
	}
}