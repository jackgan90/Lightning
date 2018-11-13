#pragma once
#include "FoundationExportDef.h"
#include "Container.h"
#include "Singleton.h"
#include "System.h"

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
				AddAndSortSystems(isystem);
				return system;
			}

			void Update(const EntityPtr& entity);
		private:
			void AddAndSortSystems(const SystemPtr<ISystem>& system)
			{
				auto newSystemPriority = system->GetPriority();
				bool inserted{ false };
				for (auto it = mSystems.begin();it != mSystems.end();++it)
				{
					if ((*it)->GetPriority() > newSystemPriority)
					{
						mSystems.insert(it, system);
						inserted = true;
						break;
					}
				}
				if (!inserted)
					mSystems.insert(mSystems.end(), system);
			}
			friend class Singleton<SystemManager>;
			using SystemList = Container::List<SystemPtr<ISystem>>;
			SystemManager();
			SystemList mSystems;
		};
	}
}