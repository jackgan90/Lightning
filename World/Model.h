#pragma once
#include "RenderableSpaceObject.h"
#include "IModel.h"

namespace Lightning
{
	namespace World
	{
		class Model : public RenderableSpaceObject<IModel, Model>
		{
		public:
			bool NeedRender()const override;
		};
	}
}