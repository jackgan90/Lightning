#pragma once
#include "IDrawable.h"
#include "ISerializable.h"

namespace Lightning
{
	namespace World
	{
		struct IModel : Render::IDrawable, ISerializable
		{

		};
	}
}