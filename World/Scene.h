#pragma once
#include <cstdint>
#include <vector>
#include "SpaceObject.h"
#include "IScene.h"

namespace Lightning
{
	namespace World
	{
		class Scene : public IScene, public SpaceObject<Scene>
		{
		public:
			Scene(std::uint32_t id);
			~Scene()override;
			std::uint32_t GetID()const override{ return mID; }
			void Tick()override;
			ISpaceCamera* GetActiveCamera()override;
			ISpaceCamera* CreateCamera()override;
		protected:
			std::vector<std::shared_ptr<ISpaceCamera>> mCameras;
			std::uint32_t mID;
		};
	}
}
