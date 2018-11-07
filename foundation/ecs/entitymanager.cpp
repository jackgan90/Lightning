#include "entitymanager.h"
#include "systemmanager.h"

namespace Lightning
{
	namespace Foundation
	{
		EntityManager::EntityManager() 
			: mCurrentEntityID(0) 
		{
			mIterator = mEntities.end();
		}

		void EntityManager::Update()
		{
			for (mIterator = mEntities.begin(); mIterator != mEntities.end();)
			{
				auto it = mIterator;
				SystemManager::Instance()->Update(mIterator->second);
				if (it == mIterator)
					++mIterator;
			}
		}
	}
}
