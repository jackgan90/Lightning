#pragma once
#include <cstdint>
#include "container.h"
#include "sceneexportdef.h"
#include "camera.h"
#include "drawable.h"
#include "scenerenderdata.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::container;

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
			container::vector<SharedDrawablePtr> mDrawables;
			SceneRenderData mRenderData;
		};
	}
}
