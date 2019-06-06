#pragma once
#include "IRenderable.h"
#include "ISpaceObject.h"

namespace Lightning
{
	namespace World
	{
		struct IModel : IRenderable, virtual ISpaceObject
		{

		};
	}
}