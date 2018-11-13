#include "SystemManager.h"

namespace Lightning
{
	namespace Foundation
	{
		SystemManager::SystemManager()
		{

		}

		void SystemManager::Update(const EntityPtr& entity)
		{
			for (auto it = mSystems.begin(); it != mSystems.end(); ++it)
			{
				(*it)->Update(entity);
			}
		}
	}
}