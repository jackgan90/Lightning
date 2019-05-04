#pragma once
#include <cstdint>
#include "IScene.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		class Scene : public IScene
		{
		public:
			Scene(std::uint32_t id);
			~Scene()override;
			std::uint32_t GetID()const override{ return mID; }
			void Tick()override;
			void AddDrawable(const std::shared_ptr<IDrawable>& drawable)override;
			ICamera* GetActiveCamera()override { return mActiveCamera; }
		protected:
			ICamera* mActiveCamera;
			std::uint32_t mID;
			std::vector<std::shared_ptr<IDrawable>> mDrawables;
			SceneRenderData mRenderData;
		};
	}
}
