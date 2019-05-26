#pragma once
#include "SpaceObject.h"
#include "IRenderable.h"

namespace Lightning
{
	namespace World
	{
		template<typename Interface, typename Implementation>
		class RenderableSpaceObject : public Interface, public SpaceObject<Implementation>
		{
		public:
			const Transform GetTransform()const override { return GetGlobalTransform(); }
		};
	}
}
