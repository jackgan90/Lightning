#include "systemmanager.h"

namespace Lightning
{
	namespace Foundation
	{
		SystemManager::SystemManager()
		{

		}

		void SystemManager::Update(const EntityPtr& entity)
		{
			for (mIterator = mSystems.begin(); mIterator != mSystems.end();)
			{
				(*mIterator)->Update(entity);
				if (mIterator != mSystems.end())
					++mIterator;
			}
		}
	}
}