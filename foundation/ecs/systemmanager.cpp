#include "systemmanager.h"

namespace Lightning
{
	namespace Foundation
	{
		SystemManager::SystemManager():mUpdating(false)
		{

		}

		void SystemManager::Update(const EntityPtr& entity)
		{
			mUpdating = true;
			for (auto& system : mSystems)
				system->Update(entity);
			mUpdating = false;
			for (const auto& system : mAddingSystems)
			{
				SortSystems(system);
			}
			mAddingSystems.clear();
		}
	}
}