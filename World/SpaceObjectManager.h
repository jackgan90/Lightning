#pragma once
#include <atomic>
#include "tbb/concurrent_queue.h"
#include "Singleton.h"

namespace Lightning
{
	namespace World
	{
		class SpaceObjectBase;
		class SpaceObjectManager : public Foundation::Singleton<SpaceObjectManager>
		{
		public:
			std::uint64_t GetNextSpaceObjectID();
			bool AddChild(const std::shared_ptr<SpaceObjectBase>& parent, const std::shared_ptr<SpaceObjectBase>& child);
			bool RemoveChild(const std::shared_ptr<SpaceObjectBase>& parent, const std::shared_ptr<SpaceObjectBase>& child);
			//synchronize children add/remove operations,thread unsafe,must be invoke by only one thread!
			void Synchronize();
		private:
			friend class Foundation::Singleton<SpaceObjectManager>;
			SpaceObjectManager();
			enum class Operation
			{
				Add,
				Remove
			};
			struct SpaceObjectOperation
			{
				SpaceObjectOperation() = default;
				SpaceObjectOperation(const SpaceObjectOperation&) = default;
				SpaceObjectOperation(Operation o, const std::shared_ptr<SpaceObjectBase>& p, const std::shared_ptr<SpaceObjectBase>& c)
					: operation(o), parent(p), child(c){}
				Operation operation;
				std::shared_ptr<SpaceObjectBase> parent;
				std::shared_ptr<SpaceObjectBase> child;
			};
			tbb::concurrent_queue<SpaceObjectOperation> mOperations;
			std::atomic<std::uint64_t> mNextSpaceObjectID;
		};
	}
}