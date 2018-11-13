#pragma once
#include <cstdint>
#include "container.h"
#include "SceneExportDef.h"
#include "Camera.h"
#include "Drawable.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Container;

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
			Container::Vector<SharedDrawablePtr> mDrawables;
			SceneRenderData mRenderData;
		};
	}
}
