#include <algorithm>
#include "SpaceObjectManager.h"
#include "SpaceObject.h"

namespace Lightning
{
	namespace World
	{
		std::uint64_t SpaceObjectManager::GetNextSpaceObjectID()
		{
			return mNextSpaceObjectID.fetch_add(1, std::memory_order_relaxed);
		}

		SpaceObjectManager::SpaceObjectManager() : mNextSpaceObjectID(1)
		{

		}

		bool SpaceObjectManager::AddChild(const std::shared_ptr<SpaceObjectBase>& parent, const std::shared_ptr<SpaceObjectBase>& child)
		{
			if (!child || !parent)
				return false;
			if (parent == child)
				return false;
			mOperations.emplace(Operation::Add, parent, child);
			return true;
		}

		bool SpaceObjectManager::RemoveChild(const std::shared_ptr<SpaceObjectBase>& parent, const std::shared_ptr<SpaceObjectBase>& child)
		{
			if (!child || !parent)
				return false;
			if (parent == child)
				return false;
			mOperations.emplace(Operation::Remove, parent, child);
			return true;
		}

		void SpaceObjectManager::ChangeParent(const std::shared_ptr<SpaceObjectBase>& child, const std::shared_ptr<SpaceObjectBase>& parent)
		{
			child->mParent = parent;
		}

		void SpaceObjectManager::Synchronize()
		{
			SpaceObjectOperation operation;
			while (mOperations.try_pop(operation))
			{
				if (operation.operation == Operation::Add)
				{
					operation.child->mParent = operation.parent;
					operation.parent->mChildren.push_back(operation.child);
				}
				else
				{
					operation.child->mParent.reset();
					operation.parent->mChildren.remove(operation.child);
				}
			}
		}
	}
}