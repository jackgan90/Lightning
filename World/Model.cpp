#include "Model.h"

namespace Lightning
{
	namespace World
	{
		bool Model::NeedRender()const
		{
			//Most of the time,it's not the "model" that needs render,but its children,aka meshes.
			//But sometimes, a Model do needs to render itself.For example bounding box,but that's 
			//another story.
			return false;
		}
	}
}