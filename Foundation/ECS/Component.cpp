#include "entity.h"
#include "component.h"

namespace Lightning
{
	namespace Foundation
	{
		void Component::Remove()
		{
			if (auto owner = mOwner.lock())
			{
				owner->RemoveComponent(shared_from_this());
			}
		}

		std::shared_ptr<Entity> Component::GetOwner()const
		{ 
			return mOwner.lock(); 
		}
	}
}