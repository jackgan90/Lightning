#pragma once
#include <cstdint>
#include <vector>
#include "sceneexportdef.h"
#include "camera.h"
#include "drawable.h"
#include "scenerenderdata.h"

namespace Lightning
{
	namespace Scene
	{
		class LIGHTNING_SCENE_API Scene
		{
		public:
			Scene(std::uint32_t id);
			virtual ~Scene();
			std::uint32_t GetID()const { return mID; }
			virtual void Update();
			void AddDrawable(const SharedDrawablePtr& drawable);
			Camera* GetActiveCamera() { return mActiveCamera; }
		protected:
			Camera* mActiveCamera;
			std::uint32_t mID;
			std::vector<SharedDrawablePtr> mDrawables;
			SceneRenderData mRenderData;
		};
	}
}
