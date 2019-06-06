#pragma once
#include "ISpaceObject.h"
#include "IRenderable.h"

namespace Lightning
{
	namespace World
	{
		struct IModel : IRenderable, virtual ISpaceObject
		{

		};
	}
}