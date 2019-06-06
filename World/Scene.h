#pragma once
#include <cstdint>
#include <vector>
#include "SpaceObject.h"
#include "IScene.h"

namespace Lightning
{
	namespace World
	{
		class Scene : public SpaceObject<IScene, Scene>
		{
		public:
			Scene();
			~Scene()override;
			void Tick()override;
			ISpaceCamera* GetActiveCamera()override;
			ISpaceCamera* CreateCamera()override;
		protected:
			std::vector<std::shared_ptr<ISpaceCamera>> mCameras;
		};
	}
}
