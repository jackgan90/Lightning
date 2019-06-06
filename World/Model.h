#pragma once
#include "SpaceObject.h"
#include "IModel.h"

namespace Lightning
{
	namespace World
	{
		class Model : public SpaceObject<IModel, Model>
		{
		};
	}
}