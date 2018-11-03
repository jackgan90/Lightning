#include "entitymanager.h"
#include "systemmanager.h"

namespace Lightning
{
	namespace Foundation
	{
		EntityManager::EntityManager() 
			: mCurrentEntityID(0), mUpdating(false)
		{

		}

		void EntityManager::Update()
		{
			mUpdating = true;
			for (auto it = mEntities.begin(); it != mEntities.end();++it)
			{
				if (it->second->mRemoved)
					continue;
				SystemManager::Instance()->Update(it->second);
			}
			mUpdating = false;
			//C++17 has unordered_map::merge,but c++11/14 doesn't 
			for (const auto& id : mRemovingEntities)
			{
				mEntities.erase(id);
			}
			mRemovingEntities.clear();
			for (const auto& entity : mAddingEntities)
			{
				mEntities.insert(entity);
			}
			mAddingEntities.clear();
		}
	}
}
