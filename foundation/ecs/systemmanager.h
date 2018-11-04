#pragma once
#include "foundationexportdef.h"
#include "container.h"
#include "singleton.h"
#include "system.h"

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API SystemManager : public Singleton<SystemManager>
		{
		public:
			template<typename S, typename... Args>
			SystemPtr<S> CreateSystem(Args&&... args)
			{
				static_assert(std::is_base_of<ISystem, S>::value, "S must be a subclass of ISystem!");
				auto system = std::make_shared<S>(std::forward<Args>(args)...);
				auto isystem = std::static_pointer_cast<ISystem, S>(system);
				if (mUpdating)
				{
					mAddingSystems.push_back(isystem);
				}
				else
				{
					SortSystems(isystem);
				}
				return system;
			}

			void SortSystems(const SystemPtr<ISystem>& system)
			{
				auto newSystemPriority = system->GetPriority();
				bool inserted{ false };
				for (std::size_t i = 0;i < mSystems.size();++i)
				{
					if (mSystems[i]->GetPriority() > newSystemPriority)
					{
						mSystems.insert(mSystems.begin() + i, system);
						inserted = true;
						break;
					}
				}
				if (!inserted)
					mSystems.push_back(system);
			}

			void Update(const EntityPtr<Entity>& entity);
		private:
			friend class Singleton<SystemManager>;
			SystemManager();
			container::vector<SystemPtr<ISystem>> mSystems;
			container::vector<SystemPtr<ISystem>> mAddingSystems;
			bool mUpdating;
		};
	}
}