#include "entitymanager.h"
#include "systemmanager.h"

namespace Lightning
{
	namespace Foundation
	{
		EntityManager::EntityManager() 
			: mCurrentEntityID(0), mIteratorValid(true)
		{
			mIterator = mEntities.end();
		}

		void EntityManager::Update()
		{
			for (mIterator = mEntities.begin(); mIterator != mEntities.end();)
			{
				mIteratorValid = true;
				SystemManager::Instance()->Update(mIterator->second);
				if (mIteratorValid)
					++mIterator;
			}
		}
	}
}
